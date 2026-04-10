create_precompiled_module(PANACEA-basic SHARED)

add_dependencies(PANACEA-basic
    PUBLIC
        SimpleCPP
        glm::glm-header-only
        fmt::fmt
        Tracy::TracyClient
)