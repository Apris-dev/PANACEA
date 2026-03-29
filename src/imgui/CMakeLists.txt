create_module(PANACEA-imgui SHARED)
target_include_directories(PANACEA-imgui PUBLIC "include/imgui")

find_package(Vulkan REQUIRED)
add_dependencies(PANACEA-imgui
    PRIVATE
        SDL3::SDL3
        Vulkan::Vulkan
)