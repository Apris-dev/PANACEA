create_module(PANACEA-VRI SHARED)

add_dependencies(PANACEA-VRI
    PRIVATE
        SDL3::SDL3
        vk-bootstrap::vk-bootstrap
        Vulkan::Vulkan
        dxcompiler

    PUBLIC

        PANACEA-basic
)