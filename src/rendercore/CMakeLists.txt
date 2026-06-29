add_shared_package(PANACEA-rendercore

        # Vulkan (Migrate to VRI)

        include/rendercore/VulkanResources.h
        src/rendercore/VulkanResources.cpp

        include/rendercore/VulkanUtils.h
        src/rendercore/VulkanUtils.cpp

        # Loader

        include/rendercore/EngineLoader.h
        src/rendercore/EngineLoader.cpp

        # Rendering Utils

        include/rendercore/RenderThread.h

        include/rendercore/RenderStack.h

        include/rendercore/Instancer.h
        src/rendercore/Instancer.cpp

        include/rendercore/Font.h
        src/rendercore/Font.cpp

        include/rendercore/Material.h

        include/rendercore/StaticMesh.h

        # Base Renderer

        include/rendercore/Pass.h
        src/rendercore/Pass.cpp

        include/rendercore/Renderer.h
        src/rendercore/Renderer.cpp
)

link_package(PRIVATE SDL3::SDL3)
link_package(PRIVATE fastgltf::fastgltf)
link_package(PRIVATE Freetype::Freetype)
link_package(PRIVATE basisu::basisu_encoder)
link_package(PRIVATE meshoptimizer::meshoptimizer)

link_package(PUBLIC vk-bootstrap::vk-bootstrap)
link_package(PUBLIC Vulkan::Vulkan)
link_package(PUBLIC PANACEA-basic)
link_package(PUBLIC PANACEA-VRI)

# Basis universal doesn't include itself by default
target_include_directories(PANACEA-rendercore PUBLIC
        "../../third_party/basis_universal"
)