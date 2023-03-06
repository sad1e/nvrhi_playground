#include "deferred_renderer.h"

#include <stdexcept>
#include <utility>
#include <donut/engine/View.h>

DeferredRenderer::DeferredRenderer(DeviceManager* deviceManager)
    : device_manager_(deviceManager)
{
	if (deviceManager == nullptr)
		throw std::invalid_argument("Invalid device manager.");

	graphics_command_list_ = deviceManager->GetDevice()->createCommandList();
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

	return topology_changed;
}

void DeferredRenderer::Render(nvrhi::IFramebuffer* framebuffer)
{
	int windowWidth, WindowHeight;
}