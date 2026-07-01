#include "basic/core/Common.h"
#include "editor/EditorRenderer.h"
#include "engine/Engine.h"

void run_engine() {
	// Tell Engine to use CEditorRenderer, which has certain passes
	CEngine::run<CEditorRenderer>();
}

extern "C" EXPORT int run() {

#ifdef BUILD_DEBUG
	msgs("Engine Initialized with profile 'Debug'");
#endif
#ifdef BUILD_RELEASE
	msgs("Engine Initialized with profile 'Release'");
#endif

	run_engine();

	return 0;
}
