add_shared_package(PANACEA-engine

        # Engine

        include/engine/Engine.h
        src/engine/Engine.cpp

        include/engine/EngineSettings.h
        src/engine/EngineSettings.cpp

        include/engine/Input.h
        src/engine/Input.cpp

        include/engine/Viewport.h
        src/engine/Viewport.cpp

        # Base Objects

        include/scene/base/RenderableObject.h

        include/scene/base/Scene.h
        src/scene/base/Scene.cpp

        include/scene/base/SceneObject.h
        src/scene/base/SceneObject.cpp

        # Viewport Objects

        include/scene/viewport/generic/Image.h

        include/scene/viewport/generic/Layout.h

        include/scene/viewport/generic/Text.h
        src/scene/viewport/generic/Text.cpp

        include/scene/viewport/Sprite.h

        # World Objects

        include/scene/world/Camera.h
        src/scene/world/Camera.cpp

        include/scene/world/StaticMeshObject.h
)

link_package(PRIVATE SDL3::SDL3)
link_package(PRIVATE PANACEA-rendercore)
link_package(PRIVATE PANACEA-imgui)

link_package(PUBLIC PANACEA-basic)
link_package(PUBLIC PANACEA-VRI)