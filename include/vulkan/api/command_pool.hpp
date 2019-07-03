#ifndef __VK_HELPER_COMMAND_POOL_HPP__
#define __VK_HELPER_COMMAND_POOL_HPP__

#include "vulkan/api/device.hpp"
#include "vulkan/api/queue.hpp"
#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class CommandPool {

      public:
        CommandPool(Device device, Queue queue);

        CommandPool(Device device, Queue queue, uint32_t count);

        VkResult allocateCommandBuffers(uint32_t count);

        VkCommandBuffer beginSingleTimeCommands();

        VkResult endSingleTimeCommands(VkCommandBuffer commandBuffer);

        VkCommandBuffer getCommandBuffer(size_t index);
        
        VkCommandPool getPool();

        virtual ~CommandPool();

      private:
        const Device device;
        const Queue queue;

        VkCommandPool pool = VK_NULL_HANDLE;
        std::vector<VkCommandBuffer> commandBuffers;
    };

} // namespace VKHelper

#endif //__VK_HELPER_COMMAND_POOL_HPP__