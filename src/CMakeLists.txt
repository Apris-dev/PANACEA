function(has_changed)
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${ARGV})
endfunction()

add_subdirectory(basic)
add_subdirectory(imgui)
add_subdirectory(VRI)
add_subdirectory(rendercore)
add_subdirectory(engine)
add_subdirectory(renderer)
add_subdirectory(editor)

block()
    add_package_executable(PANACEA Application.cpp)

    link_package(PUBLIC PANACEA-basic)
    link_package(PUBLIC PANACEA-engine)
    link_package(PUBLIC PANACEA-editor)
endblock()

if(WIN32)
    set_target_properties(PANACEA
            PROPERTIES
            LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
            LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup"
    )

    target_compile_definitions(PANACEA PRIVATE
            $<$<CONFIG:Debug>:BUILD_DEBUG>
            $<$<CONFIG:Release>:BUILD_RELEASE>
    )

    # Tell cmake to copy dlls to the target directory
    add_custom_command(TARGET PANACEA POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:PANACEA> $<TARGET_FILE_DIR:PANACEA>
            COMMAND_EXPAND_LISTS
    )
endif()