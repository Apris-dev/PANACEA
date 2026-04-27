create_module(PANACEA-imgui SHARED)
target_include_directories(PANACEA-imgui PUBLIC "include/imgui")

add_dependencies(PANACEA-imgui
    PRIVATE
        SDL3::SDL3
        Vulkan::Vulkan
)