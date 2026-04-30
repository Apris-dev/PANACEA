#include "VRI/resources/RenderPass.h"

TUnique<CRenderPass> VRICreateRenderPass(
    uint32 attachmentCount,
    const VkAttachmentDescription* attachments,
    uint32 subpassCount,
    const VkSubpassDescription* subpasses,
    uint32 dependencyCount,
    const VkSubpassDependency* dependencies,
    const CRenderPass::Flags flags) {
    VkRenderPassCreateInfo createInfo {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = static_cast<VkRenderPassCreateFlags>(flags),
        .attachmentCount = attachmentCount,
        .pAttachments = attachments,
        .subpassCount = subpassCount,
        .pSubpasses = subpasses,
        .dependencyCount = dependencyCount,
        .pDependencies = dependencies
   };
    TUnique<CRenderPass> renderPass;
    VK_CHECK(vkCreateRenderPass(CVRI::get()->getDevice()->device, &createInfo, nullptr, &renderPass->mRenderPass));
    //m_Allocator->m_Resources.push(commandPool);
    return std::move(renderPass);
}
