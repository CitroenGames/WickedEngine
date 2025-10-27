cmake_minimum_required(VERSION 3.19)

project(Example_ImGui)

if (NOT WIN32)
	find_package(Threads REQUIRED)
endif()

set(IMGUILIBNAME IMGUI_LIB)

set(SOURCE_FILES
	src/Example_ImGui.cpp
	src/Example_ImGui.h
)

if(WIN32)
	list(APPEND SOURCE_FILES
		src/main_Windows.cpp
		src/main_Windows.h
		win32/resource.rc
		win32/resource.h
	)
	add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
	target_include_directories(${PROJECT_NAME} PRIVATE win32)
else()
	list(APPEND SOURCE_FILES
		src/main_SDL2.cpp
	)
	add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE ImGui)

setup_wicked_app(${PROJECT_NAME})

set(IMGUI_FILES
	ImGui/imconfig.h
	ImGui/imgui.cpp
	ImGui/imgui.h
	ImGui/imgui_demo.cpp
	ImGui/imgui_draw.cpp
	ImGui/imgui_internal.h
	ImGui/imgui_tables.cpp
	ImGui/imgui_widgets.cpp
	ImGui/imstb_rectpack.h
	ImGui/imstb_textedit.h
	ImGui/imstb_truetype.h
)

if (WIN32)
	list(APPEND IMGUI_FILES
		ImGui/imgui_impl_win32.cpp
		ImGui/imgui_impl_win32.h
	)
	add_library(${IMGUILIBNAME} OBJECT ${IMGUI_FILES})

	target_link_libraries(${PROJECT_NAME}
		${IMGUILIBNAME}
	)

else()

	list(APPEND IMGUI_FILES
		ImGui/imgui_impl_sdl.cpp
		ImGui/imgui_impl_sdl.h
	)
	add_library(${IMGUILIBNAME} OBJECT ${IMGUI_FILES})
	find_package(SDL2 REQUIRED)
	target_link_libraries(${IMGUILIBNAME} PRIVATE SDL2::SDL2)

	target_link_libraries(${PROJECT_NAME}
		PUBLIC
		Threads::Threads

		PRIVATE
		${IMGUILIBNAME}
	)
endif()

# Copy shaders
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ImGuiPS.hlsl 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMAND ${CMAKE_COMMAND} -E copy_if_different 
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders/ImGuiVS.hlsl 
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    COMMENT "Copying ImGui shaders to output directory"
)

if(WICKED_USE_IPO)
set_target_properties(${PROJECT_NAME} PROPERTIES
    INTERPROCEDURAL_OPTIMIZATION ON
    INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF
)
endif()

if (MSVC)
	set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
endif()
set_property(TARGET ${PROJECT_NAME} PROPERTY UNITY_BUILD NO)