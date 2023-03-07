#include "deferred_renderer.h"

#include <stdexcept>
#include <utility>

#include <donut/core/log.h>
#include <donut/engine/View.h>
#include <donut/engine/Scene.h>
#include <donut/engine/CommonRenderPasses.h>
// #include <donut/render/GBufferFillPass.h>
#include <donut/render/DrawStrategy.h>
#include <nvrhi/nvrhi.h>

bool RenderTargets::IsUpdateRequired(uint2 size, uint sampleCount) const
{
	if (any(m_Size != size) || m_SampleCount != sampleCount)
		return true;

	return false;
}

DeferredRenderer::DeferredRenderer(DeviceManager* deviceManager, std::shared_ptr<ShaderFactory> shaderFactory)
    : device_manager_(deviceManager)
    , shader_factory_(shaderFactory)
    , binding_cache_(deviceManager->GetDevice())
{
	if (deviceManager == nullptr)
		throw std::invalid_argument("Invalid device manager.");

	nvrhi::IDevice* device = deviceManager->GetDevice();
	graphics_command_list_ = device->createCommandList();
	common_render_pass_ = std::make_shared<CommonRenderPasses>(device, shaderFactory);

	opaque_draw_strategy_ = std::make_shared<InstancedOpaqueDrawStrategy>();
}

void DeferredRenderer::Render(nvrhi::IFramebuffer* framebuffer, Scene* scene)
{
	if (scene == nullptr)
		return;

	uint32_t frame_index = device_manager_->GetFrameIndex();

	int windowWidth, windowHeight;
	device_manager_->GetWindowDimensions(windowWidth, windowHeight);

	nvrhi::Viewport windowViewport = nvrhi::Viewport(float(windowWidth), float(windowHeight));
	nvrhi::Viewport renderViewport = windowViewport;

	scene->RefreshSceneGraph(frame_index);

	bool exposureResetRequired = false;
	{
		uint width = windowWidth;
		uint height = windowHeight;

		uint sample_count = 1;

		bool need_new_passes = false;

		if (!render_targets_ || render_targets_->IsUpdateRequired(uint2(width, height), sample_count))
		{
			render_targets_ = nullptr;
			binding_cache_.Clear();

			render_targets_ = std::make_unique<RenderTargets>();
			render_targets_->Init(device_manager_->GetDevice(), uint2(width, height), sample_count, true, true);

			need_new_passes = true;
		}

		if (InitViews())
		{
			need_new_passes = true;
		}

		if (need_new_passes)
		{
			CreateRenderPasses(exposureResetRequired);
		}
	}

	graphics_command_list_->open();

	scene->RefreshBuffers(graphics_command_list_, frame_index);

	nvrhi::ITexture* framebuffer_texture = framebuffer->getDesc().colorAttachments[0].texture;
	graphics_command_list_->clearTextureFloat(framebuffer_texture, nvrhi::AllSubresources, nvrhi::Color(0.0f));

	render_targets_->Clear(graphics_command_list_);

	GBufferFillPass::Context gbuffer_context;
	RenderCompositeView(graphics_command_list_, view_.get(), prev_view_.get(), *render_targets_->GBufferFramebuffer,
	    scene->GetSceneGraph()->GetRootNode(), *opaque_draw_strategy_, *gbuffer_pass_, gbuffer_context, "GBuffer",
	    false);

	DeferredLightingPass::Inputs deferred_lighting_inputs;
	deferred_lighting_inputs.SetGBuffer( *render_targets_ );
	deferred_lighting_inputs.ambientOcclusion = nullptr;
	deferred_lighting_inputs.lights = &scene->GetSceneGraph()->GetLights();
	deferred_lighting_inputs.lightProbes = nullptr;
	deferred_lighting_inputs.output = render_targets_->ShadedColor;

	deferred_lighting_pass_->Render(graphics_command_list_, *view_, deferred_lighting_inputs);

	common_render_pass_->BlitTexture(graphics_command_list_, framebuffer, render_targets_->ShadedColor, &binding_cache_);

	graphics_command_list_->close();
	device_manager_->GetDevice()->executeCommandList(graphics_command_list_);

	std::swap(view_, prev_view_);
}

void DeferredRenderer::Destroy()
{
	if (deferred_lighting_pass_)
		deferred_lighting_pass_->ResetBindingCache();
	if (gbuffer_pass_)
		gbuffer_pass_->ResetBindingCache();

	binding_cache_.Clear();
}

bool DeferredRenderer::InitViews()
{
	float2 rtsize = float2(render_targets_->GetSize());

	float2 pixel_offset = float2(0.0f);

	std::shared_ptr<PlanarView> planar_view = std::dynamic_pointer_cast<PlanarView, IView>(view_);

	dm::affine3 view_matrix;
	float vfov = dm::radians(camera_vertical_fov_);
	float znear = 0.01f;

	view_matrix = camera_.GetWorldToViewMatrix();

	bool topology_changed = false;

	if (!planar_view)
	{
		view_ = planar_view = std::make_shared<PlanarView>();
		prev_view_ = std::make_shared<PlanarView>();
		topology_changed = true;
	}

	float4x4 projection_matrix = perspProjD3DStyleReverse(vfov, rtsize.x / rtsize.y, znear);

	planar_view->SetViewport(nvrhi::Viewport(rtsize.x, rtsize.y));
	planar_view->SetPixelOffset(pixel_offset);
	planar_view->SetMatrices(view_matrix, projection_matrix);

	planar_view->UpdateCache();

	dm::float3 camera_position = view_matrix.m_translation;
	camera_.LookAt(camera_position, camera_position + view_matrix.m_linear.row2, view_matrix.m_linear.row1);

	if (topology_changed)
	{
		*std::static_pointer_cast<PlanarView>(prev_view_) = *std::static_pointer_cast<PlanarView>(view_);
	}

	return topology_changed;
}

void DeferredRenderer::CreateRenderPasses(bool& exposureResetRequired)
{
	uint32_t motion_vector_stencil_mask = 0x01;

	nvrhi::IDevice* device = device_manager_->GetDevice();

	GBufferFillPass::CreateParameters gbuffer_params;
	gbuffer_params.enableMotionVectors = false;
	gbuffer_params.stencilWriteMask = motion_vector_stencil_mask;
	gbuffer_pass_ = std::make_unique<GBufferFillPass>(device, common_render_pass_);
	gbuffer_pass_->Init(*shader_factory_, gbuffer_params);

	deferred_lighting_pass_ = std::make_unique<DeferredLightingPass>(device, common_render_pass_);
	deferred_lighting_pass_->Init(shader_factory_);
}