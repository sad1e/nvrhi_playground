#include "playground_ui.h"
#include "playground_app.h"

#include <donut/engine/ShaderFactory.h>


PlaygroundApp::PlaygroundApp(DeviceManager* devmgr)
    : Super(devmgr)
{

	auto native_fs = std::make_shared<NativeFileSystem>();
	auto media_path = GetDirectoryWithExecutable().parent_path() / "media";
	auto framework_shader_path = GetDirectoryWithExecutable() / "shaders/framework" / GetShaderTypeName(GetDevice()->getGraphicsAPI());

	root_fs_ = std::make_shared<RootFileSystem>();
	root_fs_->mount("/media", media_path);
	root_fs_->mount("/shaders/donut", framework_shader_path);
	root_fs_->mount("/native", native_fs);

	shader_factory_ = std::make_shared<ShaderFactory>(GetDevice(), root_fs_, "/shaders");
}

PlaygroundUI::PlaygroundUI(DeviceManager* devmgr, std::shared_ptr<PlaygroundApp> app)
    : Super(devmgr)
{}
