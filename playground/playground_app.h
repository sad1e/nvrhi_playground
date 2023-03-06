#pragma once

#include <donut/app/ApplicationBase.h>
#include <donut/app/Camera.h>
#include <donut/app/DeviceManager.h>

#include "playground_forward.h"

using namespace donut::app;
using namespace donut::engine;
using namespace donut::vfs;

class PlaygroundApp : public ApplicationBase
{
	using Super = ApplicationBase;

public:
	PlaygroundApp(DeviceManager* devmgr);

	virtual bool LoadScene(std::shared_ptr<IFileSystem> fs, const std::filesystem::path& sceneFileName) override { return false; }

	std::shared_ptr<ShaderFactory> GetShaderFactory() { return shader_factory_; }

private:
	std::shared_ptr<RootFileSystem> root_fs_;
	std::shared_ptr<ShaderFactory>  shader_factory_;
};