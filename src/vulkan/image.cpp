#include "vulkan/vk_helper.hpp"

#include <optional>

namespace VKHelper {

    static VkResult createImage(VkDevice device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage &image) {

        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = extent.width;
        imageInfo.extent.height = extent.height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = tiling;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK_RET(vkCreateImage(device, &imageInfo, nullptr, &image));
        return VK_SUCCESS;
    }

    static VkResult createImageView(VkDevice device, VkImage image, VkFormat format, VkImageAspectFlags aspect, VkImageView &imageView) {

        VkImageViewCreateInfo viewInfo = {};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = image;
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = format;
        viewInfo.subresourceRange.aspectMask = aspect;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        VK_CHECK_RET(vkCreateImageView(device, &viewInfo, nullptr, &imageView));
        return VK_SUCCESS;
    }

    Image::Image(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                 VkImageAspectFlags aspect)
        : logicalDevice(physicalDevice, device), extent(extent), format(format), tiling(tiling), usage(usage), aspect(aspect) {
        VK_CHECK_FAIL(createImage(device, extent, format, tiling, usage, image), "image creation failed");
        VK_CHECK_FAIL(createImageView(device, image, format, aspect, imageView), "image view creation failed");
    }

    Image::Image(VkPhysicalDevice physicalDevice, VkDevice device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage,
                 VkImageAspectFlags aspect, VkMemoryPropertyFlags properties)
        : logicalDevice(physicalDevice, device), extent(extent), format(format), tiling(tiling), usage(usage), aspect(aspect) {
        VK_CHECK_FAIL(createImage(device, extent, format, tiling, usage, image), "image creation failed");
        VK_CHECK_FAIL(createImageView(device, image, format, aspect, imageView), "image view creation failed");
        VK_CHECK_FAIL(bind(properties), "buffer bind failed");
    }

    VkResult Image::bind(VkMemoryPropertyFlags properties) {

        VK_CHECK_NOT_NULL(logicalDevice.phyDev, "physicalDevice is VK_NULL_HANDLE");
        VK_CHECK_NULL(memory, "buffer is already binded to memory");

        this->memProperties = properties;
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(logicalDevice.dev, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        std::optional memType = findMemoryType(logicalDevice.phyDev, memRequirements.memoryTypeBits, properties);
        if (memType) {
            allocInfo.memoryTypeIndex = *memType;
        } else {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        VK_CHECK_RET(vkAllocateMemory(logicalDevice.dev, &allocInfo, nullptr, &memory));
        VK_CHECK_RET(vkBindImageMemory(logicalDevice.dev, image, memory, 0));

        return VK_SUCCESS;
    }

    VkImageView Image::getView() {
        return imageView;
    }

    Image::~Image() {
        vkDestroyImageView(logicalDevice.dev, imageView, nullptr);
        vkDestroyImage(logicalDevice.dev, image, nullptr);
        if (memory) {
            vkFreeMemory(logicalDevice.dev, memory, nullptr);
        }
    }

} // namespace VKHelper
