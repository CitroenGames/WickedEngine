#include "stdafx.h"
#include <SDL2/SDL.h>
#include "log.h"

class game : public wi::Application
{
public:
    void Initialize() override;
	void Update(float dt) override;

};

void game::Initialize()
{
	wi::Application::Initialize();
	wi::initializer::InitializeComponentsImmediate();

	///wi::scene::LoadModel("Content/scripts/character_controller/assets/level.wiscene");

    LOG_TRACE("Application initialized");

	wi::lua::RunFile("Content/scripts/character_controller/character_controller.lua");
}

void game::Update(float dt)
{
    wi::Application::Update(dt);
}

int sdl_loop(wi::Application &application)
{
    SDL_Event event;

    bool quit = false;
    while (!quit)
    {
        SDL_PumpEvents();
        application.Run();

        while( SDL_PollEvent(&event)) 
        {
            switch (event.type) 
            {
                case SDL_QUIT:      
                    quit = true;
                    break;
                case SDL_WINDOWEVENT:
                    switch (event.window.event) 
                    {
                    case SDL_WINDOWEVENT_CLOSE:
                        quit = true;
                        break;
                    case SDL_WINDOWEVENT_RESIZED:
                        application.SetWindow(application.window);
                        break;
                    default:
                        break;
                    }
                default:
                    break;
            }
        }
    }

    return 0;

}

int main(int argc, char *argv[])
{
    INIT_LOG();
    game application;
    #ifdef WickedEngine_SHADER_DIR
    wi::renderer::SetShaderSourcePath(WickedEngine_SHADER_DIR);
    #endif

    application.infoDisplay.active = true;
    application.infoDisplay.watermark = true;
    application.infoDisplay.resolution = true;
    application.infoDisplay.fpsinfo = true;

    sdl2::sdlsystem_ptr_t system = sdl2::make_sdlsystem(SDL_INIT_EVERYTHING | SDL_INIT_EVENTS);
    sdl2::window_ptr_t window = sdl2::make_window(
            "Template",
            SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            2560, 1440,
            SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN | SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_Event event;

    application.SetWindow(window.get());

    application.Initialize();

    int ret = sdl_loop(application);

    SDL_Quit();

    application.Shutdown();
    return ret;
}
