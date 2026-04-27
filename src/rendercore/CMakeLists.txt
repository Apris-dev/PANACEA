create_module(PANACEA-rendercore SHARED)

add_dependencies(PANACEA-rendercore
    PRIVATE
        SDL3::SDL3
        fastgltf::fastgltf
        Freetype::Freetype
        basisu::basisu_encoder
        meshoptimizer::meshoptimizer
    PUBLIC
        vk-bootstrap::vk-bootstrap
        Vulkan::Vulkan

        PANACEA-basic
        PANACEA-VRI
)

# Basis universal doesn't include itself by default
target_include_directories(PANACEA-rendercore PUBLIC
        "../../third_party/basis_universal"
)