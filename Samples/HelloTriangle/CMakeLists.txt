cmake_minimum_required(VERSION 3.19)

project(HelloTriangle)

file(GLOB_RECURSE SOURCE_FILES "src/*.cpp" "src/*.h" "src/*.hpp")

if(WIN32)
    list (APPEND SOURCE_FILES
            win32/resource.rc
            win32/resource.h
    )
    # add win32 path to include directories
    include_directories(win32)
    add_executable(${PROJECT_NAME} WIN32 ${SOURCE_FILES})
else()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES})
endif()

setup_wicked_app(${PROJECT_NAME})

target_precompile_headers(${PROJECT_NAME} PRIVATE src/stdafx.h)

# Copy Content directory to build output
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Content
        $<TARGET_FILE_DIR:${PROJECT_NAME}>/Content
    COMMENT "Copying Content directory to build output"
)