#pragma once

#include <donut/app/imgui_renderer.h>

using namespace donut::app;
using namespace donut::engine;
using namespace donut::vfs;

class PlaygroundApp;

class PlaygroundUI : public ImGui_Renderer
{
	using Super = ImGui_Renderer;
public:
	PlaygroundUI(DeviceManager* devmgr, std::shared_ptr<PlaygroundApp> app);

	virtual void buildUI() override {}

private:
};