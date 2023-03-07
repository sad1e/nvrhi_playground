#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include <donut/core/log.h>
#include <nvrhi/utils.h>

#include "playground_ui.h"
#include "playground_app.h"

using namespace donut;
using namespace donut::app;
using namespace donut::engine;
using namespace donut::vfs;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	nvrhi::GraphicsAPI api = GetGraphicsAPIFromCommandLine(__argc, __argv);

	DeviceCreationParameters device_params;

	device_params.backBufferWidth = 1280;
	device_params.backBufferHeight = 720;
	device_params.swapChainSampleCount = 1;
	device_params.swapChainBufferCount = 3;
	device_params.startFullscreen = false;
	device_params.vsyncEnabled = true;

	std::string sceneName;

	DeviceManager* device_manager = DeviceManager::Create(api);
	const char* api_string = nvrhi::utils::GraphicsAPIToString(device_manager->GetGraphicsAPI());

	std::string window_title = "Playground (" + std::string(api_string) + ")";

	if (!device_manager->CreateWindowDeviceAndSwapChain(device_params, window_title.c_str()))
	{
		log::error("Cannot initialize a %s graphics device with the requested parameters", api_string);
		return 1;
	}

	{
		std::shared_ptr<PlaygroundApp> demo = std::make_shared<PlaygroundApp>(device_manager);
		std::shared_ptr<PlaygroundUI> gui = std::make_shared<PlaygroundUI>(device_manager, demo);

		gui->Init(demo->GetShaderFactory());

		device_manager->AddRenderPassToBack(demo.get());
		device_manager->AddRenderPassToBack(gui.get());

		device_manager->RunMessageLoop();
	}

	device_manager->Shutdown();

#ifdef _DEBUG
	device_manager->ReportLiveObjects();
#endif
	delete device_manager;

	return 0;
}