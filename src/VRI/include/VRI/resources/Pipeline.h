#pragma once

#include "sstl/Vector.h"
#include "VRI/resources/VRIResources.h"

struct CPipelineLayout;

enum class EBlendMode : uint8 {
    NONE,
    ADDITIVE,
    ALPHA_BLEND
};

enum class EDepthTestMode : uint8 {
    NORMAL,
    BEHIND,
    FRONT
};

struct SPipelineCreateInfo {
    VkShaderModule vertexModule;
    VkShaderModule fragmentModule;
    VkPrimitiveTopology mTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    VkPolygonMode mPolygonMode = VK_POLYGON_MODE_FILL;
    float mLineWidth = 1.f;
    VkCullModeFlags mCullMode = VK_CULL_MODE_FRONT_BIT;
    VkFrontFace mFrontFace = VK_FRONT_FACE_CLOCKWISE;
    bool mUseMultisampling = false;
    EBlendMode mBlendMode = EBlendMode::NONE;
    EDepthTestMode mDepthTestMode = EDepthTestMode::NORMAL;
    VkFormat mColorFormat;
    VkFormat mDepthFormat;
};

class CVertexAttributeArchive {

    struct VertexAttributeFormat {
        VkVertexInputRate inputRate;
        TVector<VkFormat> formats;
    };

public:

    void createBinding(const VkVertexInputRate InputRate) {
        m_Formats.push(VertexAttributeFormat{InputRate, TVector<VkFormat>{}});
    }

    VkPipelineVertexInputStateCreateInfo get() {
        m_Bindings.clear();
        m_Attributes.clear();

        uint32 location = 0;
        uint32 binding = 0;
        for (auto& [inputRate, formats] : m_Formats) {
            uint32 stride = 0;
            for (uint32 current = 0; current < formats.getSize(); ++current, ++location) {
                const auto& format = formats[current];
                m_Attributes.push(VkVertexInputAttributeDescription{
                    location,
                    binding,
                    format,
                    stride
                });
                //stride += getVkFormatSize(format);
            }
            m_Bindings.push({
                .binding = binding,
                .stride = stride,
                .inputRate = inputRate
            });
            binding++;
        }

        return
        {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
            .vertexBindingDescriptionCount = static_cast<uint32>(m_Bindings.getSize()),
            .pVertexBindingDescriptions = m_Bindings.data(),
            .vertexAttributeDescriptionCount = static_cast<uint32>(m_Attributes.getSize()),
            .pVertexAttributeDescriptions = m_Attributes.data()
        };
    }

    friend CVertexAttributeArchive& operator<<(CVertexAttributeArchive& inArchive, VkFormat inFormat) {
        inArchive.m_Formats.top().formats.push(inFormat);
        return inArchive;
    }

private:

    // Stored here because the data needs to last the lifetime of this object
    TVector<VkVertexInputBindingDescription> m_Bindings;
    TVector<VkVertexInputAttributeDescription> m_Attributes;

    TVector<VertexAttributeFormat> m_Formats;
};

//TODO: rename
struct SPipeline : SVRIResource {

    SPipeline() = default;

    [[nodiscard]] VkPipeline get() const { return mPipeline; }

    [[nodiscard]] EXPORT VkPipelineLayout getLayout() const;

    EXPORT void bind(VkCommandBuffer cmd, const VkPipelineBindPoint inBindPoint) const;

    EXPORT virtual std::function<void()> getDestroyer() override;

private:
    EXPORT friend TUnique<SPipeline> VRICreatePipeline(const SPipelineCreateInfo&, CVertexAttributeArchive&, const TUnique<CPipelineLayout>&);
    VkPipeline mPipeline = nullptr;
    TFrail<CPipelineLayout> mLayout;
};

EXPORT TUnique<SPipeline> VRICreatePipeline(const SPipelineCreateInfo& inCreateInfo, CVertexAttributeArchive& inAttributes, const TUnique<CPipelineLayout>& inLayout);
