add_shared_package(PANACEA-imgui

        # Config
        include/imgui/imconfig.h
        src/imgui/imgui_demo.cpp

        # Internal
        include/imgui/imgui_internal.h

        include/imgui/imgui_stdlib.h
        src/imgui/imgui_stdlib.cpp

        include/imgui/imgui_impl_sdl3.h
        src/imgui/imgui_impl_sdl3.cpp

        include/imgui/imgui_impl_vulkan.h
        src/imgui/imgui_impl_vulkan.cpp

        # Drawing

        include/imgui/imstb_rectpack.h

        include/imgui/imstb_textedit.h

        include/imgui/imstb_truetype.h

        src/imgui/imgui_draw.cpp

        src/imgui/imgui_tables.cpp

        src/imgui/imgui_widgets.cpp

        # Main

        include/imgui/imgui.h
        src/imgui/imgui.cpp
)

# Insure include directory is set properly for imgui
target_include_directories(PANACEA-imgui PUBLIC "include/imgui")

link_package(PRIVATE SDL3::SDL3)
link_package(PRIVATE Vulkan::Vulkan)