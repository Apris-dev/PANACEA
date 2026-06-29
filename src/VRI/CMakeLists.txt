add_shared_package(PANACEA-VRI

        # VRI

        include/VRI/VRIUtils.h

        include/VRI/BindlessResources.h
        src/VRI/BindlessResouces.cpp

        include/VRI/VRI.h
        src/VRI/VRI.cpp

        include/VRI/VRIAllocator.h
        src/VRI/VRIAllocator.cpp

        include/VRI/VRICommands.h
        src/VRI/VRICommands.cpp

        include/VRI/VRISwapchain.h
        src/VRI/VRISwapchain.cpp

        # Resources

        include/VRI/resources/CommandPool.h
        src/VRI/resources/CommandPool.cpp

        include/VRI/resources/DescriptorPool.h
        src/VRI/resources/DescriptorPool.cpp

        include/VRI/resources/DescriptorSet.h
        src/VRI/resources/DescriptorSet.cpp

        include/VRI/resources/DescriptorSetLayout.h
        src/VRI/resources/DescriptorSetLayout.cpp

        include/VRI/resources/Fence.h
        src/VRI/resources/Fence.cpp

        include/VRI/resources/Pipeline.h
        src/VRI/resources/Pipeline.cpp

        include/VRI/resources/PipelineLayout.h
        src/VRI/resources/PipelineLayout.cpp

        include/VRI/resources/RenderPass.h
        src/VRI/resources/RenderPass.cpp

        include/VRI/resources/Sampler.h
        src/VRI/resources/Sampler.cpp

        include/VRI/resources/Semaphore.h
        src/VRI/resources/Semaphore.cpp

        include/VRI/resources/Shader.h
        src/VRI/resources/Shader.cpp

        include/VRI/resources/VRIBuffer.h
        src/VRI/resources/VRIBuffer.cpp

        include/VRI/resources/VRIImage.h
        src/VRI/resources/VRIImage.cpp

        include/VRI/resources/VRIMeshBuffer.h
        src/VRI/resources/VRIMeshBuffer.cpp

        include/VRI/resources/VRIResources.h
        src/VRI/resources/VRIResources.cpp
)

link_package(PRIVATE SDL3::SDL3)
link_package(PRIVATE vk-bootstrap::vk-bootstrap)
link_package(PRIVATE Vulkan::Vulkan)
link_package(PRIVATE Microsoft::DirectXShaderCompiler)

link_package(PUBLIC PANACEA-basic)