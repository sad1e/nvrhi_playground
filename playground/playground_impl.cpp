#include "playground_ui.h"
#include "playground_app.h"

#include <donut/engine/ShaderFactory.h>

#include "./renderer/deferred_renderer.h"


PlaygroundApp::PlaygroundApp(DeviceManager* deviceManager)
    : Super(deviceManager)
{

	auto native_fs = std::make_shared<NativeFileSystem>();
	auto media_path = GetDirectoryWithExecutable().parent_path() / "media";
	auto framework_shader_path = GetDirectoryWithExecutable() / "shaders/framework" / GetShaderTypeName(GetDevice()->getGraphicsAPI());

	root_fs_ = std::make_shared<RootFileSystem>();
	root_fs_->mount("/media", media_path);
	root_fs_->mount("/shaders/donut", framework_shader_path);
	root_fs_->mount("/native", native_fs);

	std::filesystem::path scene_path = "/media/scenes";
	available_scene_names_ = FindScenes(*root_fs_, scene_path);

	shader_factory_ = std::make_shared<ShaderFactory>(GetDevice(), root_fs_, "/shaders");

	command_list_ = GetDevice()->createCommandList();

	if (current_scene_name_.empty())
		SetCurrentScene(FindPreferredScene(available_scene_names_, "Sponza.gltf"));

	deferred_renderer_ = std::make_unique<DeferredRenderer>(GetDeviceManager());
}

void PlaygroundApp::SetCurrentScene(const std::string& sceneName)
{
	if (current_scene_name_ == sceneName)
		return;
	
	current_scene_name_ = sceneName;

	BeginLoadingScene(root_fs_, current_scene_name_);
}

void PlaygroundApp::RenderScene(nvrhi::IFramebuffer* framebuffer)
{
	command_list_->open();

	nvrhi::ITexture* framebuffer_texture = framebuffer->getDesc().colorAttachments[0].texture;
	command_list_->clearTextureFloat(framebuffer_texture, nvrhi::AllSubresources, nvrhi::Color(0.4f, 0.2f, 0.6f, 1.0f));

	command_list_->close();
	GetDevice()->executeCommandList(command_list_);

	GetDeviceManager()->SetVsyncEnabled(false);
}

PlaygroundUI::PlaygroundUI(DeviceManager* devmgr, std::shared_ptr<PlaygroundApp> app)
    : Super(devmgr)
{}


void PlaygroundUI::buildUI()
{
	const auto& io = ImGui::GetIO();

	ImGui::SetWindowPos(ImVec2(10.0f, 10.0f), 0);

	{ // Settings window.
		ImGui::Begin("Settings", 0, ImGuiWindowFlags_AlwaysAutoResize);

		ImGui::Text("Renderer: %s", GetDeviceManager()->GetRendererString());

		double frameTime = GetDeviceManager()->GetAverageFrameTimeSeconds();
		if (frameTime > 0.0)
			ImGui::Text("%.3f ms/frame (%.1f FPS)", frameTime * 1e3, 1.0 / frameTime);

		ImGui::End();
	}
}