#include "app.h"
#include <wiECS.h>
#include <wiScene_Components.h>
#include <iostream>
#include <wiGUI.h>

void SampleApp::Initialize()
{
	wi::Application::Initialize();
	render.Load();

	ActivatePath(&render);
}

// scene and renderer stuff

wi::gui::Label label_test;

#define CAMERAMOVESPEED 10.0f
static float camera_pos[3] = { 0.0f, 1.0f, -3.0f };
static float camera_ang[3] = { 8.0f, 0.0f, 0.0f };
wi::scene::TransformComponent camera_transform;

static bool mouse_down = false;

void init_sample_scene() // you can load .wiscene file instead of doing it like this
{
	wi::scene::Scene& scene = wi::scene::GetScene();

	// Ambient light
	auto& weather = scene.weathers.Create(wi::ecs::CreateEntity());
	weather.ambient = XMFLOAT3(0.313f, 0.313f, 0.313f);

	// SpotLight
	{
		auto lightEntity = scene.Entity_CreateLight("SpotLight", XMFLOAT3(2, 2, -2));
		auto& light = *scene.lights.GetComponent(lightEntity);
		light.SetType(wi::scene::LightComponent::LightType::SPOT);
		light.intensity = 20.0f;
		light.SetCastShadow(true);
		light.SetVisualizerEnabled(true);

		wi::scene::TransformComponent& transform = *scene.transforms.GetComponent(lightEntity);
		// -70 degree to radians:
		// -70 × (π / 180) ≈ -1,22173
		// -40 degree to radians:
		// -40 × (π / 180) ≈ -0,69813
		// I will round them to -1.2 and -0.7
		transform.RotateRollPitchYaw(XMFLOAT3(-1.2f, -0.7f, 0.0f));
		transform.UpdateTransform();
	}

	// Directional Light
	{
		auto lightEntity = scene.Entity_CreateLight("DirLight", XMFLOAT3(0, 3, 0));
		auto& light = *scene.lights.GetComponent(lightEntity);
		light.SetType(wi::scene::LightComponent::LightType::DIRECTIONAL);
		light.intensity = 1.0f;
		light.color = XMFLOAT3(0.333f, 0.314f, 0.353f);
		light.SetCastShadow(true);
		// light.SetVisualizerEnabled(true);
	}

	// Ground
	{
		auto groundEntity = scene.Entity_CreatePlane("ground_entity");
		auto groundComponent = scene.transforms.GetComponent(groundEntity);
		groundComponent->Scale(XMFLOAT3(10, 0, 10));
	}

	// Grid
	{
		wi::renderer::SetToDrawGridHelper(true);
	}
}

wi::gui::Label label;
wi::gui::Button run;
void init_gui(SampleRenderPath& srp)
{
	wi::gui::GUI& gui = srp.GetGUI();

	label.Create("Label1");
	label.SetText("Ten Minute Physics: 10 - Soft Body Simulation");
	label.font.params.h_align = wi::font::WIFALIGN_CENTER;
	label.SetSize(XMFLOAT2(340, 20));
	gui.AddWidget(&label);

	static wi::gui::Label label2;
	static wi::gui::Button restart;
	static wi::gui::Button squash;
	static wi::gui::Button newBody;
	static wi::gui::Slider compliance;
	static wi::gui::Slider direction;

	run.Create("runButton");
	run.SetText("Run Simulation");
	run.SetSize(XMFLOAT2(200, 20));
	run.SetPos(XMFLOAT2(90, 140));
	run.OnClick(
		[&](wi::gui::EventArgs args)
		{
		});
	gui.AddWidget(&run);

	restart.Create("restartButton");
	restart.SetText("Restart Simulation");
	restart.SetSize(XMFLOAT2(200, 20));
	restart.SetPos(XMFLOAT2(90, 170));
	restart.OnClick(
		[&](wi::gui::EventArgs args)
		{
		});
	gui.AddWidget(&restart);

	squash.Create("squashButton");
	squash.SetText("Squash");
	squash.SetSize(XMFLOAT2(200, 20));
	squash.SetPos(XMFLOAT2(90, 200));
	squash.OnClick(
		[&](wi::gui::EventArgs args)
		{
		});
	gui.AddWidget(&squash);

	newBody.Create("newBodyButton");
	newBody.SetText("Add Soft Body");
	newBody.SetSize(XMFLOAT2(200, 20));
	newBody.SetPos(XMFLOAT2(90, 230));
	newBody.OnClick(
		[&](wi::gui::EventArgs args)
		{
		});
	gui.AddWidget(&newBody);

	compliance.Create(0, 100, 20, 100, "slider1");
	compliance.SetText("Compliance: ");
	compliance.SetSize(XMFLOAT2(200, 20));
	compliance.SetPos(XMFLOAT2(90, 260));
	compliance.OnSlide(
		[](wi::gui::EventArgs args)
		{
			
		});
	gui.AddWidget(&compliance);

	label_test.Create("Label3");
	label_test.font.params.h_align = wi::font::WIFALIGN_CENTER;
	label_test.SetSize(XMFLOAT2(200, 20));
	label_test.SetPos(XMFLOAT2(90, 290));
	label_test.SetColor(wi::Color(0, 120, 0));
	gui.AddWidget(&label_test);

	label2.Create("Label2");
	label2.SetText("WASD - Move Camera\n"
		"[R]Mouse - Rotate Camera\n"
		"[L]Mouse - Pick Object");
	label2.font.params.h_align = wi::font::WIFALIGN_CENTER;
	label2.SetSize(XMFLOAT2(200, 60));
	label2.SetPos(XMFLOAT2(90, 320));
	gui.AddWidget(&label2);
}


void SampleRenderPath::Load()
{
	init_sample_scene();

	wi::RenderPath3D::Load();
}

void SampleRenderPath::Update(float dt)
{
	// --- CAMERA CONTROL ---
	{
		wi::scene::CameraComponent& camera = wi::scene::GetCamera();
		// XMFLOAT4 currentMouse = wi::input::GetPointer();
		static XMFLOAT4 originalMouse = XMFLOAT4(0, 0, 0, 0);
		static bool camControlStart = true;
		if (camControlStart)
		{
			originalMouse = wi::input::GetPointer();
		}

		if (wi::input::Down(wi::input::MOUSE_BUTTON_MIDDLE) ||
			wi::input::Down(wi::input::MOUSE_BUTTON_RIGHT))
		{
			camControlStart = false;
			// Mouse delta
			float xDif = wi::input::GetMouseState().delta_position.x;
			float yDif = wi::input::GetMouseState().delta_position.y;
			// float xDif = currentMouse.x - originalMouse.x;
			// float yDif = currentMouse.y - originalMouse.y;
			xDif = 0.1f * xDif;
			yDif = 0.1f * yDif;
			wi::input::SetPointer(originalMouse);
			wi::input::HidePointer(true);
			camera_ang[0] += yDif;
			camera_ang[1] += xDif;

			if (camera_ang[0] < -89.999f)
				camera_ang[0] = -89.999f;

			if (camera_ang[0] > 89.999f)
				camera_ang[0] = 89.999f;
		}
		else
		{
			camControlStart = true;
			wi::input::HidePointer(false);
		}

		float movespeed = CAMERAMOVESPEED;
		if (wi::input::Down(wi::input::KEYBOARD_BUTTON_LSHIFT) ||
			wi::input::Down(wi::input::KEYBOARD_BUTTON_RSHIFT))
		{
			movespeed *= 3.0f;
		}
		movespeed *= dt;

		if (wi::input::Down((wi::input::BUTTON)'W') ||
			wi::input::Down(wi::input::KEYBOARD_BUTTON_UP))
		{
			camera_pos[0] += movespeed * camera.At.x;
			camera_pos[1] += movespeed * camera.At.y;
			camera_pos[2] += movespeed * camera.At.z;
		}
		if (wi::input::Down((wi::input::BUTTON)'S') ||
			wi::input::Down(wi::input::KEYBOARD_BUTTON_DOWN))
		{
			camera_pos[0] -= movespeed * camera.At.x;
			camera_pos[1] -= movespeed * camera.At.y;
			camera_pos[2] -= movespeed * camera.At.z;
		}

		XMFLOAT3 dir_right;
		XMStoreFloat3(&dir_right, camera.GetRight());
		if (wi::input::Down((wi::input::BUTTON)'D') ||
			wi::input::Down(wi::input::KEYBOARD_BUTTON_RIGHT))
		{
			camera_pos[0] += movespeed * dir_right.x;
			camera_pos[1] += movespeed * dir_right.y;
			camera_pos[2] += movespeed * dir_right.z;
		}
		if (wi::input::Down((wi::input::BUTTON)'A') ||
			wi::input::Down(wi::input::KEYBOARD_BUTTON_LEFT))
		{
			camera_pos[0] -= movespeed * dir_right.x;
			camera_pos[1] -= movespeed * dir_right.y;
			camera_pos[2] -= movespeed * dir_right.z;
		}

		camera_transform.ClearTransform();
		camera_transform.Translate(
			XMFLOAT3(camera_pos[0],
				camera_pos[1],
				camera_pos[2]));
		camera_transform.RotateRollPitchYaw(
			XMFLOAT3(wi::math::DegreesToRadians(camera_ang[0]),
				wi::math::DegreesToRadians(camera_ang[1]),
				wi::math::DegreesToRadians(camera_ang[2]))
		);
		camera_transform.UpdateTransform();
		camera.TransformCamera(camera_transform);
		camera.UpdateCamera();
	}

    RenderPath3D::Update(dt);
}

