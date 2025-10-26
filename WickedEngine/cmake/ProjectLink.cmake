# Macro to setup a WickedEngine application
# Usage: setup_wicked_app(MyProjectName)
macro(setup_wicked_app PROJECT_NAME)
    # THIS IS A HACK BUT WORKS FOR NOW: THIS IS ALREADY SET IN WICKEDENGINE's CMakeLists.txt BUT ITS NOT VISIBLE HERE
    if(WIN32)
        set(LIBDXCOMPILER "dxcompiler.dll")
        set(COPY_OR_SYMLINK_DIR_CMD "copy_directory")
        # Prevent Windows.h from defining min/max macros
        target_compile_definitions(${PROJECT_NAME} PRIVATE NOMINMAX)
    else()
        set(LIBDXCOMPILER "libdxcompiler.so")
        set(COPY_OR_SYMLINK_DIR_CMD "symlink_directory")
    endif()
    # Link against WickedEngine
    target_link_libraries(${PROJECT_NAME} WickedEngine)
    
    # Copy dxcompiler DLL next to the executable
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different 
            ${WICKED_ROOT_DIR}/WickedEngine/${LIBDXCOMPILER} 
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
        COMMENT "Copying ${LIBDXCOMPILER} to output directory for ${PROJECT_NAME}"
    )

    if(WIN32 AND MSVC)
        # Enable multi-processor compilation
        target_compile_options(${PROJECT_NAME} PRIVATE /MP)
        
        # Set optimization flags for Release builds
        target_compile_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:/O2>        # Maximum optimization
            $<$<CONFIG:Release>:/Ob2>       # Inline function expansion
            $<$<CONFIG:Release>:/Oi>        # Enable intrinsic functions
            $<$<CONFIG:Release>:/Ot>        # Favor fast code
            $<$<CONFIG:Release>:/GL>        # Whole program optimization
        )
        
        # Set linker flags for Release builds
        target_link_options(${PROJECT_NAME} PRIVATE
            $<$<CONFIG:Release>:/LTCG>      # Link-time code generation
            $<$<CONFIG:Release>:/OPT:REF>   # Remove unreferenced functions/data
            $<$<CONFIG:Release>:/OPT:ICF>   # Identical COMDAT folding
        )
    endif()
endmacro()