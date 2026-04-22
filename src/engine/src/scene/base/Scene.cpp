#include "scene/base/Scene.h"

#include "sarch/FileArchive.h"
#include "scene/world/Camera.h"

void CScene::init() {
	std::filesystem::path path = SPaths::get()->mAssetPath.string() + "Scene.scn";

	if (std::filesystem::exists(path)) {
		CFileArchive<EOpenType::BINARY_READ> file(path.string());
		file >> *this;
		file.close();
	}

	mMainCamera = TShared<CCamera>{};
}

void CScene::destroy() {
	mMainCamera.destroy();

	std::filesystem::path path = SPaths::get()->mAssetPath.string() + "Scene.scn";

	CFileArchive<EOpenType::BINARY_WRITE> file(path.string());
	file << *this;
	file.close();
}

void CScene::update() {
	mMainCamera->update();
}
