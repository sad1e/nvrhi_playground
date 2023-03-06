#pragma once

#include <donut/app/DeviceManager.h>
#include <donut/app/Camera.h>
#include <donut/render/GBuffer.h>

using namespace donut::app;
using namespace donut::engine;
using namespace donut::math;
using namespace donut::render;

namespace donut::engine
{
class IView;
}

class RenderTargets : public GBufferRenderTargets
{
public:
	void Init(nvrhi::IDevice* device, dm::uint2 size, dm::uint sampleCount, bool enableMotionVectors, bool useReverseProjection) override
	{
		GBufferRenderTargets::Init(device, size, sampleCount, enableMotionVectors, useReverseProjection);
	}
};

class DeferredRenderer
{
public:
	DeferredRenderer(DeviceManager* deviceManager);
	DeferredRenderer(std::nullptr_t) = delete;

	bool InitViews();

	void Render(nvrhi::IFramebuffer* framebuffer);

private:
	nvrhi::CommandListHandle graphics_command_list_;
	// nvrhi::CommandListHandle compute_command_list_;
	// nvrhi::CommandListHandle copy_command_list_;

	std::unique_ptr<RenderTargets> render_targets_;
	DeviceManager* device_manager_;

	std::shared_ptr<IView> view_;
	std::shared_ptr<IView> prev_view_;
    FirstPersonCamera camera_;
    float camera_vertical_fov_ = 60.0f;
};