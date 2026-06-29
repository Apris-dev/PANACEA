add_shared_package(PANACEA-editor
        include/editor/assetBrowser/AssetBrowser.h
        src/editor/assetBrowser/AssetBrowser.cpp

        include/editor/EditorRenderer.h
        src/editor/EditorRenderer.cpp
)

link_package(PUBLIC PANACEA-basic)
link_package(PUBLIC PANACEA-rendercore)
link_package(PUBLIC PANACEA-renderer)