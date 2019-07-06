#include "vulkan/base/simple_view.hpp"

#include <array>

#include "vulkan/api/pipeline_factory.hpp"
#include "vulkan/api/render_pass_factory.hpp"
#include "vulkan/base/simple_pipeline.hpp"
#include "vulkan/base/simple_render_pass.hpp"

namespace Qulkan::Vulkan {

    SimpleView::SimpleView(VkInstance instance, VKHelper::Device aDevice, VKHelper::Queue graphicsQueue, VkExtent2D anExtent, VkFormat aFormat,
                           const char *viewName)
        : RenderView(viewName, anExtent.width, anExtent.height, ViewType::VULKAN), device(aDevice), format(aFormat), extent(anExtent), commandPool(aDevice, graphicsQueue, 1) ,
          renderer(instance, aDevice, graphicsQueue, anExtent, aFormat),
          vertexBuffer(aDevice, sizeof(ColoredVertex) * vertices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT),
          indexBuffer(aDevice, sizeof(uint16_t) * indices.size(), VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                      VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {

        // Create render pass and pipeline
        renderPass = VKHelper::RenderPassFactory::createRenderPass<SimpleRenderPass>(aDevice, SimpleRenderPass{aDevice, aFormat});
        VK_CHECK_NOT_NULL(renderPass);
        VKHelper::PipelineInfo info =
            VKHelper::PipelineFactory::createGraphicPipeline<ColoredVertex, SimplePipeline>(aDevice, ColoredVertex{}, SimplePipeline{anExtent},
                                                                                            renderPass); // @TODO: ugly...
        pipeline = info.pipeline;
        pipelineLayout = info.layout;
        VK_CHECK_NOT_NULL(pipeline);

        // Create framebuffer
        VK_CHECK_FAIL(createFramebuffer(), "failed to create framebuffer");

        // Fill vertex buffer (staging buffer destroyed when constructor returns)
        VkDeviceSize vertexSize = sizeof(ColoredVertex) * vertices.size();
        VKHelper::Buffer stageVertex{aDevice, vertexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
        VK_CHECK_FAIL(stageVertex.mapAndCopy(vertices.data(), vertexSize), "failed to map staging vertex buffer");
        VK_CHECK_FAIL(stageVertex.copyTo(vertexBuffer, commandPool), "failed to copy staging vertex buffer to final buffer");

        // Fill index buffer (staging buffer destroyed when constructor returns)
        VkDeviceSize indexSize = sizeof(uint16_t) * indices.size();
        VKHelper::Buffer stageIndex{aDevice, indexSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};
        VK_CHECK_FAIL(stageIndex.mapAndCopy(indices.data(), indexSize), "failed to map staging index buffer");
        VK_CHECK_FAIL(stageIndex.copyTo(indexBuffer, commandPool), "failed to copy staging index buffer to final buffer");

        // Create command buffer
        commandBuffer = commandPool.getCommandBuffer(0);
        VK_CHECK_FAIL(createCommandBuffer(), "failed to create command buffer");
    }

    void SimpleView::init() {}

    void SimpleView::render(int actualRenderWidth, int actualRenderHeight) {
        VK_CHECK_FAIL(renderer.drawFrame(std::vector<VkCommandBuffer>{commandBuffer}), "failed to draw new frame");
    }

    VkResult SimpleView::createFramebuffer() {
        auto attachments = renderer.getFramebufferAttachments();

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = extent.width;
        framebufferInfo.height = extent.height;
        framebufferInfo.layers = 1;

        return vkCreateFramebuffer(device.logical, &framebufferInfo, nullptr, &framebuffer);
    }

    VkResult SimpleView::createCommandBuffer() {

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool.getPool();
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VK_CHECK_RET(vkAllocateCommandBuffers(device.logical, &allocInfo, &commandBuffer));

        // Starting command buffer recording
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

        VK_CHECK_RET(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        // Start render pass
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = extent;
        std::array<VkClearValue, 2> clearValues = {};
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        // Begin render pass
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        // Bind the graphics pipeline
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        // Bind the vertex buffer
        VkBuffer vertexBuffers[] = {vertexBuffer.getBuffer()};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        // Bind index buffer
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer.getBuffer(), 0, VK_INDEX_TYPE_UINT16);
        // Draw
        vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
        // End of render pass
        vkCmdEndRenderPass(commandBuffer);

        // End of command buffer recording
        return vkEndCommandBuffer(commandBuffer);
    }

    void SimpleView::clean() {}

    ImTextureID SimpleView::getNewTexture() {
        render(0, 0);
        return renderer.getReadbackTexture();
    }

    SimpleView::~SimpleView() {
        vkDestroyFramebuffer(device.logical, framebuffer, nullptr);
        vkDestroyPipeline(device.logical, pipeline, nullptr);
        vkDestroyPipelineLayout(device.logical, pipelineLayout, nullptr);
        vkDestroyRenderPass(device.logical, renderPass, nullptr);
    }

} // namespace Qulkan::Vulkan