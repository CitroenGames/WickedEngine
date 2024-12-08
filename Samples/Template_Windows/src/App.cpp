#include "App.h"

void App::Initialize()
{
	wi::Application::Initialize();
	wi::initializer::InitializeComponentsImmediate();

	wi::scene::LoadModel("Content/scripts/character_controller/assets/level.wiscene");
	//wi::lua::RunFile("Content/scripts/character_controller/character_controller.lua");

	player.Init();
	player.SetCapturePointer(true);
}

void App::Update(float dt)
{
	wi::Application::Update(dt);
	player.Update(dt);
}
