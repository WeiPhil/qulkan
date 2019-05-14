#ifndef __VULKAN_HELPERS_H__
#define __VULKAN_HELPERS_H__

#include <assert.h>
#include <iostream>
#include <optional>
#include <vector>

#include <vulkan/vulkan.h>

#define ASSERT_MSG(b, err_str)                                                                                                                                 \
    {                                                                                                                                                          \
        if (!(b)) {                                                                                                                                            \
            std::cout << "[FATAL]: " << err_str << "\n in " << __FILE__ << " at line " << __LINE__ << std::endl;                                               \
            assert(b);                                                                                                                                         \
        }                                                                                                                                                      \
    }

#define VK_CHECK_NULL(handle)                                                                                                                         \
    {                                                                                                                                                          \
        if (handle != VK_NULL_HANDLE) {                                                                                                                        \
            std::cout << "[FATAL]: " << #handle << " isn't VK_NULL_HANDLE" << "\n in " << __FILE__ << " at line " << __LINE__ << std::endl;                                               \
            assert(handle == VK_NULL_HANDLE);                                                                                                                  \
        }                                                                                                                                                      \
    }

#define VK_CHECK_NOT_NULL(handle)                                                                                                                     \
    {                                                                                                                                                          \
        if (handle == VK_NULL_HANDLE) {                                                                                                                        \
            std::cout << "[FATAL]: " << #handle << " is VK_NULL_HANDLE" << "\n in " << __FILE__ << " at line " << __LINE__ << std::endl;                                               \
            assert(handle != VK_NULL_HANDLE);                                                                                                                  \
        }                                                                                                                                                      \
    }

#define VK_CHECK_FAIL(b, err_str)                                                                                                                              \
    {                                                                                                                                                          \
        VkResult res = (b);                                                                                                                                    \
        if (res != VK_SUCCESS) {                                                                                                                               \
            std::cout << "[FATAL]: " << err_str << "\n in " << __FILE__ << " at line " << __LINE__ << std::endl;                                               \
            assert(res == VK_SUCCESS);                                                                                                                         \
        }                                                                                                                                                      \
    }

#define VK_CHECK_RET(b)                                                                                                                                        \
    {                                                                                                                                                          \
        VkResult res = (b);                                                                                                                                    \
        if (res != VK_SUCCESS) {                                                                                                                               \
            return res;                                                                                                                                        \
        }                                                                                                                                                      \
    }

#define VK_CHECK_RET_NULL(b)                                                                                                                                   \
    {                                                                                                                                                          \
        VkResult res = (b);                                                                                                                                    \
        if (res != VK_SUCCESS) {                                                                                                                               \
            return VK_NULL_HANDLE;                                                                                                                             \
        }                                                                                                                                                      \
    }

namespace VKHelper {

    std::optional<uint32_t> findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    struct Device {
        VkPhysicalDevice physical;
        VkDevice logical;

        Device(VkPhysicalDevice physicalDevice, VkDevice device) : physical(physicalDevice), logical(device){};
        Device(const Device &device) : physical(device.physical), logical(device.logical){};
    };

    struct Queue {
        VkQueue queue;
        uint32_t family;

        Queue(VkQueue queue, uint32_t family) : queue(queue), family(family){};
        Queue(const Queue &queue) : queue(queue.queue), family(queue.family){};
    };

    class Buffer {

      private:
        Device device;
        VkDeviceSize bufferSize;
        VkBufferUsageFlags usage;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkMemoryPropertyFlags memProperties = 0;

      public:
        Buffer(Device device, VkDeviceSize size, VkBufferUsageFlags usage);

        Buffer(Device device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

        VkResult bind(VkMemoryPropertyFlags properties);

        VkResult mapAndCopy(void *dataToCopy, size_t size);

        ~Buffer();
    };

    class Image {

      private:
        Device device;
        VkExtent2D extent;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkImageAspectFlags aspect;

        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkMemoryPropertyFlags memProperties = 0;

      public:
        Image(Device device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect);

        Image(Device device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect,
              VkMemoryPropertyFlags properties);

        VkResult bind(VkMemoryPropertyFlags properties);

        const VkImageView getView();

        ~Image();
    };

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

#endif //__VULKAN_HELPERS_H__