function(group_sources)
    foreach(list_name IN ITEMS ${ARGN})
        foreach(source IN LISTS ${list_name})
            # Ensure the source path is absolute
            if (NOT IS_ABSOLUTE "${source}")
                get_filename_component(source_abs "${source}" ABSOLUTE "${CMAKE_CURRENT_SOURCE_DIR}")
            else()
                set(source_abs "${source}")
            endif()

            # Get the relative path of the source
            get_filename_component(source_path "${source_abs}" PATH)
            file(RELATIVE_PATH source_path_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${source_path}")
            
            # Replace '/' with '\' for Visual Studio compatibility
            string(REPLACE "/" "\\" source_path_msvc "${source_path_rel}")
            
            # Assign the file to its corresponding source group
            source_group("${source_path_msvc}" FILES "${source_abs}")
        endforeach()
    endforeach()
endfunction()


macro(add_build_configuration)
    # Parse function arguments
    set(options "")
    set(oneValueArgs NAME BASE)
    set(multiValueArgs COMPILE_DEFINITIONS COMPILE_OPTIONS LINK_OPTIONS)
    cmake_parse_arguments(CONFIG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    # Validate required arguments
    if(NOT DEFINED CONFIG_NAME)
        message(FATAL_ERROR "add_build_configuration: NAME parameter is required")
    endif()

    # Add the new configuration type
    set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} ${CONFIG_NAME}
        CACHE STRING "Build configurations" FORCE)

    # If a base configuration is specified, inherit its settings
    if(DEFINED CONFIG_BASE)
        string(TOUPPER ${CONFIG_BASE} BASE_CONFIG_UPPER)
        string(TOUPPER ${CONFIG_NAME} CONFIG_NAME_UPPER)
        
        # Inherit compile flags
        set(CMAKE_C_FLAGS_${CONFIG_NAME_UPPER} "${CMAKE_C_FLAGS_${BASE_CONFIG_UPPER}}"
            CACHE STRING "C flags for ${CONFIG_NAME} configuration" FORCE)
        set(CMAKE_CXX_FLAGS_${CONFIG_NAME_UPPER} "${CMAKE_CXX_FLAGS_${BASE_CONFIG_UPPER}}"
            CACHE STRING "C++ flags for ${CONFIG_NAME} configuration" FORCE)
            
        # Inherit all linker flags
        set(CMAKE_EXE_LINKER_FLAGS_${CONFIG_NAME_UPPER} "${CMAKE_EXE_LINKER_FLAGS_${BASE_CONFIG_UPPER}}"
            CACHE STRING "Exe linker flags for ${CONFIG_NAME} configuration" FORCE)
        set(CMAKE_SHARED_LINKER_FLAGS_${CONFIG_NAME_UPPER} "${CMAKE_SHARED_LINKER_FLAGS_${BASE_CONFIG_UPPER}}"
            CACHE STRING "Shared linker flags for ${CONFIG_NAME} configuration" FORCE)
        set(CMAKE_MODULE_LINKER_FLAGS_${CONFIG_NAME_UPPER} "${CMAKE_MODULE_LINKER_FLAGS_${BASE_CONFIG_UPPER}}"
            CACHE STRING "Module linker flags for ${CONFIG_NAME} configuration" FORCE)
    endif()

    # Add any additional compile definitions
    if(CONFIG_COMPILE_DEFINITIONS)
        string(TOUPPER ${CONFIG_NAME} CONFIG_NAME_UPPER)
        foreach(def ${CONFIG_COMPILE_DEFINITIONS})
            add_compile_definitions($<$<CONFIG:${CONFIG_NAME}>:${def}>)
        endforeach()
    endif()

    # Add any additional compile options
    if(CONFIG_COMPILE_OPTIONS)
        string(TOUPPER ${CONFIG_NAME} CONFIG_NAME_UPPER)
        foreach(opt ${CONFIG_COMPILE_OPTIONS})
            add_compile_options($<$<CONFIG:${CONFIG_NAME}>:${opt}>)
        endforeach()
    endif()

    # Add any additional link options
    if(CONFIG_LINK_OPTIONS)
        string(TOUPPER ${CONFIG_NAME} CONFIG_NAME_UPPER)
        foreach(opt ${CONFIG_LINK_OPTIONS})
            add_link_options($<$<CONFIG:${CONFIG_NAME}>:${opt}>)
        endforeach()
    endif()

    # Make sure the configuration type gets propagated to the cache
    set_property(CACHE CMAKE_CONFIGURATION_TYPES PROPERTY STRINGS ${CMAKE_CONFIGURATION_TYPES})
endmacro()

# Example usage:
# add_build_configuration(
#     NAME Retail
#     BASE Release
# )

# Example usage:
# add_build_configuration(
#     NAME Retail
#     BASE RelWithDebInfo
#     COMPILE_DEFINITIONS
#         RETAIL_BUILD
#         ENABLE_TELEMETRY
#     COMPILE_OPTIONS
#         -O3
#         -march=native
#     LINK_OPTIONS
#         -s  # Strip symbols
# )