#ifndef __BASE_VK_RENDERER_HPP__
#define __BASE_VK_RENDERER_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "imgui.h"
#include "imgui/imgui_impl_vulkan.h"

#include "vulkan/api/vk_helper.hpp"

namespace Base {

    class VKRenderer {

      public:
        VKRenderer(VkInstance instance, VKHelper::Device device, VKHelper::Queue graphicsQueue, VkExtent2D extent, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

        virtual VkResult createRenderPass();
        virtual VkResult createFramebuffer();
        virtual VkResult createReadbackSampler();
        virtual VkResult drawFrame(std::vector<VkCommandBuffer> &commandBuffers);

        ~VKRenderer();

      protected:
        // Passed during creation
        VkInstance instance;           // Destruction managed by ImGUI implementation
        VKHelper::Device device;       // Destruction managed by ImGUI implementation
        VKHelper::Queue graphicsQueue; // Destruction managed by ImGUI implementation
        VkExtent2D extent;
        VkFormat format;

        // Render pass
        VkRenderPass renderPass;

        // Command pool
        VKHelper::CommandPool commandPool;

        // Depth buffer
        VKHelper::Image depthImage;

        // Drawing
        VKHelper::Image drawImage;
        VkFramebuffer drawFramebuffer;

        // Synchronization
        VKHelper::Fence fence;

        // Readback
        VKHelper::Image readbackImage;
        VkSampler readbackSampler;
        ImTextureID readbackDescriptor;
    };

} // namespace Base

#endif //__BASE_VK_RENDERER_HPP__