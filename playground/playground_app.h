#pragma once

#include <donut/app/ApplicationBase.h>
#include <donut/app/Camera.h>
#include <donut/app/DeviceManager.h>
#include <donut/engine/View.h>

#include "playground_forward.h"

using namespace donut::app;
using namespace donut::engine;
using namespace donut::vfs;

class PlaygroundApp : public ApplicationBase
{
	using Super = ApplicationBase;

public:
	PlaygroundApp(DeviceManager* devmgr);

	void SetCurrentScene(const std::string& sceneName);

	virtual bool LoadScene(std::shared_ptr<IFileSystem> fs, const std::filesystem::path& sceneFileName) override { return true; }

	virtual void RenderScene(nvrhi::IFramebuffer* framebuffer) override;

	std::shared_ptr<ShaderFactory> GetShaderFactory() { return shader_factory_; }

private:


	std::shared_ptr<RootFileSystem> root_fs_;

	std::vector<std::string> available_scene_names_;
	std::string current_scene_name_;


	std::shared_ptr<ShaderFactory> shader_factory_;

	nvrhi::CommandListHandle command_list_;
	std::shared_ptr<IView> curr_view_;
	std::shared_ptr<IView> prev_view_;
};