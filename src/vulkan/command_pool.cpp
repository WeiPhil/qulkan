#include "vulkan/vk_helper.hpp"

namespace VKHelper {

    CommandPool::CommandPool(Device device, Queue queue) : device(device), queue(queue) {

        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queue.family;

        VK_CHECK_FAIL(vkCreateCommandPool(device.dev, &poolInfo, nullptr, &pool), "command pool creation failed");
    }

    VkCommandBuffer CommandPool::beginSingleTimeCommands() {
        
        VK_CHECK_NULL(singleTimeCommandBuffer);

        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = pool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VK_CHECK_RET_NULL(vkAllocateCommandBuffers(device.dev, &allocInfo, &commandBuffer));

        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        VK_CHECK_RET_NULL(vkBeginCommandBuffer(commandBuffer, &beginInfo));

        return commandBuffer;
    }

    VkResult CommandPool::endSingleTimeCommands(VkCommandBuffer commandBuffer) {

        ASSERT_MSG(commandBuffer == singleTimeCommandBuffer, "argument buffer is invalid");
        singleTimeCommandBuffer = VK_NULL_HANDLE;

        // End recording
        VkResult ret;
        if ( (ret = vkEndCommandBuffer(commandBuffer)) != VK_SUCCESS) {
            vkFreeCommandBuffers(device.dev, pool, 1, &commandBuffer);
            return ret;
        }

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        // Submit to the queue
        if ( (ret = vkQueueSubmit(queue.queue, 1, &submitInfo, VK_NULL_HANDLE)) != VK_SUCCESS) {
            vkFreeCommandBuffers(device.dev, pool, 1, &commandBuffer);
            return ret;
        }

        // Wait for the queue to idle
        ret = vkQueueWaitIdle(queue.queue);
        vkFreeCommandBuffers(device.dev, pool, 1, &commandBuffer);
        return ret;
    }
} // namespace VKHelper
