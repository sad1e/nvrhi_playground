#pragma once

#include <donut/app/DeviceManager.h>
#include <donut/app/Camera.h>
#include <donut/render/GBuffer.h>
#include <donut/engine/BindingCache.h>

namespace donut::render
{
class GBufferFillPass;
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
	void Init(nvrhi::IDevice* device,
	    dm::uint2 size,
	    dm::uint sampleCount,
	    bool enableMotionVectors,
	    bool useReverseProjection) override
	{
		GBufferRenderTargets::Init(device, size, sampleCount, enableMotionVectors, useReverseProjection);
	}

	[[nodiscard]] bool IsUpdateRequired(uint2 size, uint sampleCount) const;
};

class DeferredRenderer
{
public:
	DeferredRenderer(DeviceManager* deviceManager, ShaderFactory* shaderFactory);
	DeferredRenderer(std::nullptr_t) = delete;

	bool InitViews();

	void Render(nvrhi::IFramebuffer* framebuffer, Scene* scene);

private:
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
	std::shared_ptr<GBufferFillPass> gbuffer_pass_;
};