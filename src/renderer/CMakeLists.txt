create_module(PANACEA-renderer SHARED)

add_dependencies(PANACEA-renderer
    PRIVATE
        vk-bootstrap::vk-bootstrap
        SDL3::SDL3

        PANACEA-imgui
    PUBLIC
        PANACEA-basic
        PANACEA-rendercore
        #PANACEA-VRI
        PANACEA-engine
)

# Basis universal doesn't include itself by default
target_include_directories(PANACEA-renderer PUBLIC
        "../../third_party/basis_universal"
)