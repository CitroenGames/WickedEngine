if (WICKED_LINUX_TEMPLATE)
    add_subdirectory(Template_Linux)
endif()

if (WICKED_WINDOWS_TEMPLATE)
    add_subdirectory(Template_Windows)
endif()

if (WICKED_TESTS)
    add_subdirectory(Tests)
endif()

if (WICKED_IMGUI_EXAMPLE)
    add_subdirectory(Example_ImGui)
    add_subdirectory(Example_ImGui_Docking)
endif()

if(USE_SDL2)
add_subdirectory(Example_SDL_Windows)
endif()