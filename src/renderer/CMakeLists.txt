add_shared_package(PANACEA-renderer

        # Rendering

        include/renderer/EngineTextures.h
        src/renderer/EngineTextures.cpp

        include/renderer/VulkanRenderer.h
        src/renderer/VulkanRenderer.cpp

        # Object Renderers

        include/renderer/object/ObjectRenderer.h
        src/renderer/object/ObjectRenderer.cpp

        include/renderer/object/StaticMeshObjectRenderer.h
        src/renderer/object/StaticMeshObjectRenderer.cpp

        # Passes

        include/renderer/passes/EngineUIPass.h
        src/renderer/passes/EngineUIPass.cpp

        include/renderer/passes/MeshPass.h
        src/renderer/passes/MeshPass.cpp

        include/renderer/passes/SpritePass.h
        src/renderer/passes/SpritePass.cpp
)

link_package(PRIVATE vk-bootstrap::vk-bootstrap)
link_package(PRIVATE SDL3::SDL3)
link_package(PRIVATE PANACEA-imgui)

link_package(PUBLIC PANACEA-basic)
link_package(PUBLIC PANACEA-rendercore)
#link_package(PUBLIC PANACEA-VRI)
link_package(PUBLIC PANACEA-engine)

# Basis universal doesn't include itself by default
target_include_directories(PANACEA-renderer PUBLIC
        "../../third_party/basis_universal"
)