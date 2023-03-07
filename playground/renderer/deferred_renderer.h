#pragma once

#include <donut/app/DeviceManager.h>
#include <donut/app/Camera.h>
#include <donut/render/GBuffer.h>
#include <donut/render/GBufferFillPass.h>
#include <donut/render/DeferredLightingPass.h>
#include <donut/engine/BindingCache.h>

namespace donut::render
{
class InstancedOpaqueDrawStrategy;
} // namespace donut::render

namespace donut::engine
{
class IView;
class Scene;
class BindingCache;
class ShaderFactory;
} // namespace donut::engine

using namespace donut::app;
using namespace donut::engine;
using namespace donut::math;
using namespace donut::render;

class RenderTargets : public GBufferRenderTargets
{
public:

	nvrhi::TextureHandle ShadedColor;

	void Init(nvrhi::IDevice* device,
	    dm::uint2 size,
	    dm::uint sampleCount,
	    bool enableMotionVectors,
	    bool useReverseProjection) override
	{
		GBufferRenderTargets::Init(device, size, sampleCount, enableMotionVectors, useReverseProjection);

		nvrhi::TextureDesc texture_desc;
        texture_desc.dimension = nvrhi::TextureDimension::Texture2D;
        texture_desc.initialState = nvrhi::ResourceStates::UnorderedAccess;
        texture_desc.keepInitialState = true;
        texture_desc.debugName = "ShadedColor";
        texture_desc.isUAV = true;
        texture_desc.format = nvrhi::Format::RGBA16_FLOAT;
        texture_desc.width = size.x;
        texture_desc.height = size.y;
        texture_desc.sampleCount = sampleCount;

        ShadedColor = device->createTexture(texture_desc);
	}

	[[nodiscard]] bool IsUpdateRequired(uint2 size, uint sampleCount) const;
};

class DeferredRenderer
{
public:
	DeferredRenderer(DeviceManager* deviceManager, std::shared_ptr<ShaderFactory> shaderFactory);

	void Render(nvrhi::IFramebuffer* framebuffer, Scene* scene);

	void Destroy();

private:
	bool InitViews();

    void CreateRenderPasses(bool& exposureResetRequired);

private:
	nvrhi::CommandListHandle graphics_command_list_;
	// nvrhi::CommandListHandle compute_command_list_;
	// nvrhi::CommandListHandle copy_command_list_;

	std::unique_ptr<RenderTargets> render_targets_;
	DeviceManager* device_manager_;

    std::shared_ptr<ShaderFactory> shader_factory_;

	BindingCache binding_cache_;

	std::shared_ptr<IView> view_;
	std::shared_ptr<IView> prev_view_;
	FirstPersonCamera camera_;
	float camera_vertical_fov_ = 60.0f;

    std::shared_ptr<CommonRenderPasses> common_render_pass_;
	std::shared_ptr<InstancedOpaqueDrawStrategy> opaque_draw_strategy_;
	std::unique_ptr<GBufferFillPass> gbuffer_pass_;
	std::unique_ptr<DeferredLightingPass> deferred_lighting_pass_;
};