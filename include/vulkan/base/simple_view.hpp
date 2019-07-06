#ifndef __QULKAN_VULKAN_SIMPLE_VIEW_HPP__
#define __QULKAN_VULKAN_SIMPLE_VIEW_HPP__

#include "qulkan/render_view.h"
#include "vulkan/api/device.hpp"
#include "vulkan/base/simple_renderer.hpp"
#include "vulkan/base/simple_vertex_format.hpp"

namespace Qulkan::Vulkan {

    class SimpleView final : public RenderView {

      public:
        SimpleView(VkInstance instance, VKHelper::Device aDevice, VKHelper::Queue graphicsQueue, VkExtent2D anExtent,
                   VkFormat aFormat = VK_FORMAT_R8G8B8A8_UNORM, const char *viewName = "Vulkan View");

        virtual void init();

        virtual void render(int actualRenderWidth, int actualRenderHeight);

        virtual void clean();

        ImTextureID getNewTexture();

        virtual ~SimpleView();

      private:
        const std::vector<ColoredVertex> vertices = {{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},  {{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
                                              {{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},    {{-0.5f, 0.5f, 0.0f}, {1.0f, 1.0f, 1.0f}},

                                              {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, {{0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
                                              {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},   {{-0.5f, 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}}};

        const std::vector<uint16_t> indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

        VKHelper::Device device;
        VKHelper::CommandPool commandPool;
        VKHelper::Buffer vertexBuffer;
        VKHelper::Buffer indexBuffer;
        SimpleRenderer renderer;

        VkFormat format;
        VkExtent2D extent;

        VkRenderPass renderPass = VK_NULL_HANDLE;
        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;
        VkFramebuffer framebuffer = VK_NULL_HANDLE;
        VkCommandBuffer commandBuffer = VK_NULL_HANDLE;

        VkResult createFramebuffer();
        VkResult createCommandBuffer();
    };

} // namespace Qulkan::Vulkan

#endif //__QULKAN_VULKAN_SIMPLE_VIEW_HPP__