#ifndef __BASE_VK_RENDERER_HPP__
#define __BASE_VK_RENDERER_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "imgui.h"

#include "vulkan/api/vk_helper.hpp"
#include "vulkan/base/pipeline.hpp"
#include "vulkan/base/vertices.hpp"

namespace Base {

    class VKRenderer {

      private:
        // Passed during creation
        VkInstance instance;           // Destruction managed by ImGUI implementation
        VKHelper::Device device;       // Destruction managed by ImGUI implementation
        VKHelper::Queue graphicsQueue; // Destruction managed by ImGUI implementation
        VkExtent2D extent;
        VkFormat format;

        // Render pass and pipeline
        Base::Pipeline pipeline;
        VkRenderPass renderPass;

        // Command pool
        VKHelper::CommandPool commandPool;
        VkCommandBuffer graphicsCommandBuffer;

        // Depth buffer
        VKHelper::Image depthImage;

        // Drawing
        VKHelper::Image drawImage;
        VkFramebuffer drawFramebuffer;

        // Vertex/Index buffer
        VKHelper::Buffer vertexBuffer;
        VKHelper::Buffer indexBuffer;

        // Synchronization
        VkFence fence;

        // Readback
        VKHelper::Image readbackImage;
        ImTextureID readbackDescriptor;

        VkResult createRenderPass();
        VkResult createFramebuffer();

      public:
        VKRenderer(VkInstance instance, VKHelper::Device device, VKHelper::Queue graphicsQueue,
                   VkExtent2D extent, VkFormat format = VK_FORMAT_R8G8B8A8_UNORM);

    };

} // namespace Base

#endif //__BASE_VK_RENDERER_HPP__