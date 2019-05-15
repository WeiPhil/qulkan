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

    Image::Image(Device device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect)
        : device(device), extent(extent), format(format), tiling(tiling), usage(usage), aspect(aspect) {
        VK_CHECK_FAIL(createImage(device.logical, extent, format, tiling, usage, image), "image creation failed");
        VK_CHECK_FAIL(createImageView(device.logical, image, format, aspect, imageView), "image view creation failed");
    }

    Image::Image(Device device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImageAspectFlags aspect,
                 VkMemoryPropertyFlags properties)
        : device(device), extent(extent), format(format), tiling(tiling), usage(usage), aspect(aspect) {
        VK_CHECK_FAIL(createImage(device.logical, extent, format, tiling, usage, image), "image creation failed");
        VK_CHECK_FAIL(createImageView(device.logical, image, format, aspect, imageView), "image view creation failed");
        VK_CHECK_FAIL(bind(properties), "buffer bind failed");
    }

    VkResult Image::bind(VkMemoryPropertyFlags properties) {

        VK_CHECK_NOT_NULL(device.physical);
        VK_CHECK_NULL(memory);

        this->memProperties = properties;
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device.logical, image, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        std::optional memType = findMemoryType(device.physical, memRequirements.memoryTypeBits, properties);
        if (memType) {
            allocInfo.memoryTypeIndex = *memType;
        } else {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        VK_CHECK_RET(vkAllocateMemory(device.logical, &allocInfo, nullptr, &memory));
        VK_CHECK_RET(vkBindImageMemory(device.logical, image, memory, 0));

        return VK_SUCCESS;
    }

    VkResult Image::copyBufferToImage(Buffer &srcBuffer, CommandPool &commandPool) {

        //@ TODO: include check for format features (must contain VK_FORMAT_FEATURE_TRANSFER_DST_BIT)
        //@ TODO: check that the buffer is big enough
        //@ TODO: image must be bind to memory ?
        ASSERT_MSG((srcBuffer.getUsageFlags() & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) != 0, "buffer doesn't have required usage flag");
        ASSERT_MSG((usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT) != 0, "image doesn't have required usage flag");
        ASSERT_MSG(layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL || layout == VK_IMAGE_LAYOUT_GENERAL, "image isn't in a compatible layout");
        
        VkBuffer buf = srcBuffer.getBuffer();
        VK_CHECK_NOT_NULL(buf);

        VkCommandBuffer commandBuffer = commandPool.beginSingleTimeCommands();
        VK_CHECK_NOT_NULL(commandBuffer);

        // Create buffercopy information
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;

        region.imageOffset = {0, 0, 0};
        region.imageExtent = {extent.width, extent.height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buf, image, layout, 1, &region);

        VK_CHECK_RET(commandPool.endSingleTimeCommands(commandBuffer));
        return VK_SUCCESS;
    }

    VkResult Image::transitionImageLayout(VkImageLayout newLayout, CommandPool &commandPool) {
        
        // Create memory barrier
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = layout;
        barrier.newLayout = newLayout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = image;
        if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if (hasStencilComponent(format)) {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        } else {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        // Check that the transition is allowed
        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;
        if (layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else if (layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        } else if (layout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        } else if (layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
            barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        } else {
            ASSERT_MSG(false, "unsupported layout transition")
        }

        // Create, record, and execute the command buffer
        VkCommandBuffer commandBuffer = commandPool.beginSingleTimeCommands();
        VK_CHECK_NOT_NULL(commandBuffer);

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
        VK_CHECK_RET(commandPool.endSingleTimeCommands(commandBuffer));

        layout = newLayout;
        return VK_SUCCESS;
    }

    const VkImageView Image::getView() { return imageView; }

    Image::~Image() {
        vkDestroyImageView(device.logical, imageView, nullptr);
        vkDestroyImage(device.logical, image, nullptr);
        if (memory) {
            vkFreeMemory(device.logical, memory, nullptr);
        }
    }

} // namespace VKHelper
