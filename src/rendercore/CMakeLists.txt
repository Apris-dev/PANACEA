create_module(PANACEA-rendercore SHARED)

find_package(Vulkan REQUIRED)
add_dependencies(PANACEA-rendercore
    PRIVATE
        SDL3::SDL3
        fastgltf::fastgltf
        freetype
        basisu_encoder
        meshoptimizer
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