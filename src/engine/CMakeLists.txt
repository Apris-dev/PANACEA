create_module(PANACEA-engine SHARED)

add_dependencies(PANACEA-engine
    PRIVATE
        SDL3::SDL3

        PANACEA-rendercore
        PANACEA-imgui
    PUBLIC
        PANACEA-basic
        PANACEA-VRI
)