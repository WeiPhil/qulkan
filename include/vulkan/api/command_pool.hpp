#ifndef __VK_HELPER_COMMAND_POOL_HPP__
#define __VK_HELPER_COMMAND_POOL_HPP__

#include "vulkan/api/device.hpp"
#include "vulkan/api/queue.hpp"
#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class CommandPool {

      private:
        Device device;
        Queue queue;

        VkCommandPool pool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;
        VkCommandBuffer singleTimeCommandBuffer = VK_NULL_HANDLE;

      public:
        CommandPool(Device device, Queue queue);

        CommandPool(Device device, Queue queue, uint32_t count);

        VkResult allocateCommandBuffers(uint32_t count);

        VkCommandBuffer beginSingleTimeCommands();

        VkResult endSingleTimeCommands(VkCommandBuffer commandBuffer);

        const VkCommandBuffer getCommandBuffer(size_t index);

        ~CommandPool();
    };

} // namespace VKHelper

#endif //__VK_HELPER_COMMAND_POOL_HPP__