create_module(PANACEA-VRI SHARED)

find_package(Vulkan REQUIRED COMPONENTS dxc)
add_dependencies(PANACEA-VRI
    PRIVATE
        SDL3::SDL3
        vk-bootstrap::vk-bootstrap
        Vulkan::Vulkan
        Vulkan::dxc_lib

    PUBLIC

        PANACEA-basic
)