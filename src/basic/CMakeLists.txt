add_shared_package(PANACEA-basic #TODO: precompile

        # Generic
        include/basic/core/Common.h
        include/basic/core/Paths.h

        # Basic Types
        include/basic/core/Class.h

        include/basic/core/Object.h
        src/basic/core/Object.cpp

        include/basic/core/Hierarchy.h

        include/basic/core/Singleton.h
        src/basic/core/Singleton.cpp

        # Registration
        include/basic/core/Factory.h
        include/basic/core/Registry.h

        # Unused (for now)
        include/basic/Profiling.h
        src/basic/Profiling.cpp

        include/basic/control/ResourceManager.h
        src/basic/control/ResourceManager.cpp
)

link_package(PUBLIC SimpleCPP)
link_package(PUBLIC glm::glm-header-only)
link_package(PUBLIC fmt::fmt)
link_package(PUBLIC Tracy::TracyClient)