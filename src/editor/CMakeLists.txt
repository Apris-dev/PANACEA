create_module(PANACEA-editor SHARED)

add_dependencies(PANACEA-editor
    PUBLIC
        PANACEA-basic
        PANACEA-rendercore
        PANACEA-renderer
)