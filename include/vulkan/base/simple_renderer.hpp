#ifndef __QULKAN_VULKAN_SIMPLE_RENDERER_HPP__
#define __QULKAN_VULKAN_SIMPLE_RENDERER_HPP__

#include "vulkan/api/command_pool.hpp"
#include "vulkan/api/device.hpp"
#include "vulkan/api/fence.hpp"
#include "vulkan/api/image.hpp"
#include "vulkan/api/queue.hpp"
#include "vulkan/readback_texture.hpp"

namespace Qulkan::Vulkan {

    class SimpleRenderer {

      public:
        SimpleRenderer(VkInstance anInstance, VKHelper::Device aDevice, VKHelper::Queue graphicsQueue, VkExtent2D anExtent,
                       VkFormat aFormat = VK_FORMAT_R8G8B8A8_UNORM);

        VkResult drawFrame(const std::vector<VkCommandBuffer> &commandBuffers);

        std::vector<VkImageView> getFramebufferAttachments();
        ImTextureID getReadbackTexture();

        ~SimpleRenderer();

      private:
        // Passed during creation
        VkInstance instance;           // Destruction managed by ImGUI implementation
        VKHelper::Device device;       // Destruction managed by ImGUI implementation
        VKHelper::Queue graphicsQueue; // Destruction managed by ImGUI implementation // @TODO: need ?
        VkExtent2D extent;
        VkFormat format;

        // Drawing
        VKHelper::Image drawImage;

        // Depth buffer
        VKHelper::Image depthImage;

        // Synchronization
        VKHelper::Fence fence;

        // Readback
        ReadbackTexture readback;
    };

} // namespace Qulkan::Vulkan

#endif //__QULKAN_VULKAN_SIMPLE_RENDERER_HPP__