#include "vulkan/base/simple_renderer.hpp"

#include <array>

namespace Qulkan::Vulkan {

    SimpleRenderer::SimpleRenderer(VkInstance anInstance, VKHelper::Device aDevice, VKHelper::Queue aGraphicsQueue, VkExtent2D anExtent, VkFormat aFormat)
        : instance(anInstance), device(aDevice), graphicsQueue(aGraphicsQueue), format(aFormat), fence(aDevice), extent(anExtent),
          depthImage(aDevice, anExtent, aDevice.findDepthFormat(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                     VK_IMAGE_ASPECT_DEPTH_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
          drawImage(aDevice, anExtent, aFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                    VK_IMAGE_ASPECT_COLOR_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
          readback(aDevice, aGraphicsQueue, anExtent, aFormat) {

        // Transition depth image to correct layout
        VKHelper::CommandPool pool{aDevice, aGraphicsQueue};
        VK_CHECK_FAIL(depthImage.transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, pool),
                      "failed to transition layout of depth image");
    }

    VkResult SimpleRenderer::drawFrame(const std::vector<VkCommandBuffer> &commandBuffers) {
        // Submit the command buffers
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        submitInfo.pCommandBuffers = commandBuffers.data();

        // Reset the fence, submit it, and then wait for it to be signaled
        VkFence waitOnFence = fence.getFence();
        VK_CHECK_RET(vkResetFences(device.logical, 1, &waitOnFence));
        VK_CHECK_RET(vkQueueSubmit(graphicsQueue.queue, 1, &submitInfo, waitOnFence));
        return vkWaitForFences(device.logical, 1, &waitOnFence, VK_TRUE, std::numeric_limits<uint64_t>::max());
    }

    std::vector<VkImageView> SimpleRenderer::getFramebufferAttachments() { return std::vector<VkImageView>{drawImage.getView(), depthImage.getView()}; }
    ImTextureID SimpleRenderer::getReadbackTexture() {return readback.updateTexture(drawImage);}

    SimpleRenderer::~SimpleRenderer() {}

} // namespace Qulkan::Vulkan