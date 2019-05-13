#ifndef __VULKAN_HELPERS_H__
#define __VULKAN_HELPERS_H__

#include <assert.h>
#include <iostream>
#include <optional>

#include <vulkan/vulkan.h>

#define ASSERT_MSG(b, err_str)                                                                                                                                 \
    {                                                                                                                                                          \
        if (!(b)) {                                                                                                                                              \
            std::cout << "[FATAL]: " << err_str << "\n in " << __FILE__ << " at line " << __LINE__ << std::endl;                                               \
            assert(b);                                                                                                                                         \
        }                                                                                                                                                      \
    }

#define VK_CHECK_NULL(handle, err_str)                                                                                                                         \
    {                                                                                                                                                          \
        if (handle != VK_NULL_HANDLE) {                                                                                                                        \
            std::cout << "[FATAL]: " << err_str << "\n in " << __FILE__ << " at line " << __LINE__ << std::endl;                                               \
            assert(handle == VK_NULL_HANDLE);                                                                                                                  \
        }                                                                                                                                                      \
    }

#define VK_CHECK_NOT_NULL(handle, err_str)                                                                                                                     \
    {                                                                                                                                                          \
        if (handle == VK_NULL_HANDLE) {                                                                                                                        \
            std::cout << "[FATAL]: " << err_str << "\n in " << __FILE__ << " at line " << __LINE__ << std::endl;                                               \
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

namespace VKHelper {

    std::optional<uint32_t> findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

    struct LogicalDevice {
        const VkPhysicalDevice phyDev;
        const VkDevice dev;

        LogicalDevice(VkPhysicalDevice physicalDevice, VkDevice device) : phyDev(physicalDevice), dev(device){};
    };

    class Buffer {

      public:
        const LogicalDevice logicalDevice;
        const VkDeviceSize bufferSize;
        const VkBufferUsageFlags usage;

      private:
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkMemoryPropertyFlags memProperties = 0;

        Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage);

        Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

        VkResult bind(VkMemoryPropertyFlags properties);

        VkResult mapAndCopy(void *dataToCopy, size_t size);

        ~Buffer();
    };

    class Image {

      public:
        const LogicalDevice logicalDevice;
        const VkExtent2D extent;
        const VkFormat format;
        const VkImageTiling tiling;
        const VkImageUsageFlags usage;
        const VkImageAspectFlags aspect;

      private:
        VkImage image = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkMemoryPropertyFlags memProperties = 0;

        Image(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect);

        Image(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect, VkMemoryPropertyFlags properties);

        VkResult bind(VkMemoryPropertyFlags properties);

        VkImageView getView();

        ~Image();
    };

} // namespace VKHelper

#endif //__VULKAN_HELPERS_H__