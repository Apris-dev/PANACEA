#pragma once

#include "cppns/container/Array.h"
#include "cppns/container/Vector.h"
#include "cppns/memory/Unique.h"

#include "VRI/resources/VRIResources.h"

struct CFence;
struct CSemaphore;
class CVRICommands;

// Forward declare vkb types
namespace vkb {
    struct Swapchain;
    template <typename T> class Result;
}

template <size_t TFrameOverlap>
struct TBuffering {

    template <typename TType>
    struct Resource {

        Resource() = default;

        TArray<TType, TFrameOverlap>& data() { return m_FrameData; }

        TType& getFrame(size_t inFrameIndex) { return m_FrameData[inFrameIndex]; }

        const TType& getFrame(size_t inFrameIndex) const { return m_FrameData[inFrameIndex]; }

        //TType& getCurrentFrame() { return getFrame(getFrameIndex()); }

        //const TType& getCurrentFrame() const { return getFrame(getFrameIndex()); }

    private:

        TArray<TType, TFrameOverlap> m_FrameData;
    };

    void incrementFrame() { m_FrameNumber++; }

    size_t getFrameNumber() const { return m_FrameNumber; };

    size_t getFrameIndex() const { return getFrameNumber() % getFrameOverlap(); }

    static size_t getFrameOverlap() { return TFrameOverlap; }

private:

    size_t m_FrameNumber = 0;

};

using CSingleBuffering = TBuffering<1>;

using CDoubleBuffering = TBuffering<2>;

struct SSwapchainImage : SVRIResource {

    SSwapchainImage() = default;

	[[nodiscard]] VkImage get() const { return mImage; }
	[[nodiscard]] VkImageView getView() const { return mImageView; }
	[[nodiscard]] VkImageLayout getLayout() const { return mLayout; }
	[[nodiscard]] uint32 getAddress() const { return mBindlessAddress; }

    EXPORT virtual std::function<void()> getDestroyer() override;

    void updateLayout(const VkImageLayout inNewLayout) {
        mLayout = inNewLayout;
    }

private:
    EXPORT friend TUnique<SSwapchainImage> VRICreateSwapchainImage(const VkImage& inImage, const VkImageView& inImageView, uint32 inBindlessAddress);

    VkImage mImage = nullptr;
    VkImageView mImageView = nullptr;
    uint32 mBindlessAddress = -1;

	VkImageLayout mLayout = VK_IMAGE_LAYOUT_UNDEFINED;
};

EXPORT TUnique<SSwapchainImage> VRICreateSwapchainImage(const VkImage& inImage, const VkImageView& inImageView, uint32 inBindlessAddress);

struct SSwapchain final : TInitializable<const vkb::Result<vkb::Swapchain>&>, IDestroyable {

    EXPORT virtual void init(const vkb::Result<vkb::Swapchain>& inSwapchainBuilder) override;

    EXPORT virtual void destroy() override;

    std::shared_ptr<vkb::Swapchain> mInternalSwapchain;

    TVector<TUnique<SSwapchainImage>> mSwapchainImages;

    // TODO: Needed if Maintenence1 is not available, should union
    // TODO: this means i probably need some sort of platform info or something
    TVector<TUnique<CSemaphore>> mSwapchainRenderSemaphores;
};

class CVRISwapchain : public TDirtyable<> {

public:

    using Buffering = CDoubleBuffering;

    struct FrameData {
        TUnique<CSemaphore> mSwapchainSemaphore = nullptr;
        TUnique<CFence> mRenderFence = nullptr;
    };

    EXPORT CVRISwapchain(struct SDL_Window* window);

    EXPORT void create(VkSwapchainKHR oldSwapchain, bool inUseVSync = true);
    EXPORT void recreate(bool inUseVSync = true);

    EXPORT virtual void destroy2();

    EXPORT TUnique<SSwapchainImage>& getSwapchainImage();

    no_discard EXPORT bool wait() const;

    EXPORT void reset() const;

    EXPORT void submit(const TFrail<CVRICommands>& inCmd, VkQueue inGraphicsQueue, uint32 inSwapchainImageIndex);

    TUnique<SSwapchain> mSwapchain = nullptr;

    VkFormat mFormat = VK_FORMAT_UNDEFINED;

    CDoubleBuffering m_Buffering{};

private:

    SDL_Window* m_Window = nullptr;

    Buffering::Resource<TUnique<FrameData>> m_Frames{};

    bool m_VSync = true;
};
