#pragma once

#include "VRI/VRIResources.h"
#include "sdg/DependencyGraph.h"
#include "sstl/List.h"

class CVRIContext {
public:

    /*
    * Create Commands
    */

    EXPORT TUnique<CFence> createFence(VkFenceCreateFlags inFlags = 0);

    TList<TFrail<SVRIResource>> resources;

};
