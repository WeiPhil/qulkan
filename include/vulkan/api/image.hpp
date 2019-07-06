#ifndef __VK_HELPER_IMAGE_HPP__
#define __VK_HELPER_IMAGE_HPP__

#include "vulkan/api/buffer.hpp"
#include "vulkan/api/command_pool.hpp"
#include "vulkan/api/device.hpp"
#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class Image {

      private:
        Device device;
        VkExtent2D extent;
        VkFormat format;
        VkImageTiling tiling;
        VkImageUsageFlags usage;
        VkImageAspectFlags aspect;

        VkImage image = VK_NULL_HANDLE;
        VkDeviceMemory memory = VK_NULL_HANDLE;
        VkImageView imageView = VK_NULL_HANDLE;
        VkMemoryPropertyFlags memProperties = 0;

      public:
        static VkResult createImage(VkDevice device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage &image);

        static VkResult createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView &imageView);

        Image(Device device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect,
              VkMemoryPropertyFlags properties);

        VkResult bind(VkMemoryPropertyFlags properties);

        VkResult copyTo(const Image &dstImage, CommandPool &commandPool);

        VkResult copyFromBuffer(VkImageLayout layout, Buffer &buffer, CommandPool &commandPool);

        VkResult transitionImageLayout(VkImageLayout oldLayout, VkImageLayout newLayout, CommandPool &commandPool);

        VkImage getImage() const;
        VkImageView getView() const;
        VkDeviceMemory getMemory() const;

        ~Image();
    };

} // namespace VKHelper

#endif //__VK_HELPER_IMAGE_HPP__