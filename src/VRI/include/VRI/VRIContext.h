#pragma once

#include "rendercore/Pass.h"
#include "sdg/DependencyGraph.h"
#include "sstl/List.h"

class CVRIContext {

    /*
    * Create Commands
    */

    EXPORT TUnique<CFence> createFence(VkFenceCreateFlags inFlags = 0);

    TRWDependencyGraph<TFrail<CPass>, TFrail<SVRIResource>, TKahnTopologicalSort> graph;

    TList<TFrail<SVRIResource>> resources;

};
