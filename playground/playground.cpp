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

	DeviceCreationParameters deviceParams;

	deviceParams.backBufferWidth = 1280;
	deviceParams.backBufferHeight = 720;
	deviceParams.swapChainSampleCount = 1;
	deviceParams.swapChainBufferCount = 3;
	deviceParams.startFullscreen = false;
	deviceParams.vsyncEnabled = true;

	std::string sceneName;

	DeviceManager* deviceManager = DeviceManager::Create(api);
	const char* apiString = nvrhi::utils::GraphicsAPIToString(deviceManager->GetGraphicsAPI());

	std::string windowTitle = "NVRHI Playground (" + std::string(apiString) + ")";

	if (!deviceManager->CreateWindowDeviceAndSwapChain(deviceParams, windowTitle.c_str()))
	{
		log::error("Cannot initialize a %s graphics device with the requested parameters", apiString);
		return 1;
	}

	{
		std::shared_ptr<PlaygroundApp> demo = std::make_shared<PlaygroundApp>(deviceManager);
		std::shared_ptr<PlaygroundUI> gui = std::make_shared<PlaygroundUI>(deviceManager, demo);

		gui->Init(demo->GetShaderFactory());

		deviceManager->AddRenderPassToBack(demo.get());
		deviceManager->AddRenderPassToBack(gui.get());

		deviceManager->RunMessageLoop();
	}

	deviceManager->Shutdown();

#ifdef _DEBUG
	deviceManager->ReportLiveObjects();
#endif
	delete deviceManager;

	return 0;
}