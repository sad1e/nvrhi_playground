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
	PlaygroundApp(DeviceManager* deviceManager);

	void SetCurrentScene(const std::string& sceneName);

	virtual bool LoadScene(std::shared_ptr<IFileSystem> fs, const std::filesystem::path& sceneFileName) override;
	virtual void SceneLoaded() override;
	virtual void SceneUnloading() override;

	virtual void RenderScene(nvrhi::IFramebuffer* framebuffer) override;

	std::shared_ptr<ShaderFactory> GetShaderFactory() { return shader_factory_; }

	// Inputs
	virtual bool KeyboardUpdate(int key, int scancode, int action, int mods) override;
	virtual bool MousePosUpdate(double xpos, double ypos) override;
	virtual bool MouseButtonUpdate(int button, int action, int mods) override;
	virtual bool MouseScrollUpdate(double xoffset, double yoffset) override;
	virtual void Animate(float elapsedTimeSeconds) override;

private:
	std::shared_ptr<RootFileSystem> root_fs_;

	std::vector<std::string> available_scene_names_;
	std::string current_scene_name_;

	std::shared_ptr<ShaderFactory> shader_factory_;
	std::shared_ptr<DirectionalLight> sun_;

	std::shared_ptr<IView> curr_view_;
	std::shared_ptr<IView> prev_view_;

	std::unique_ptr<Scene> scene_;
	std::unique_ptr<DeferredRenderer> deferred_renderer_;
	std::shared_ptr<DirectionalLight> sun_light_;

	nvrhi::CommandListHandle command_list_;
	ThirdPersonCamera camera_;
};