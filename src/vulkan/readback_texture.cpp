#include "vulkan/readback_texture.hpp"

#include <fstream>

#include "imgui/imgui_impl_vulkan.h"

namespace Qulkan::Vulkan {

    ReadbackTexture::ReadbackTexture(VKHelper::Device aDevice, VKHelper::Queue graphicsQueue, VkExtent2D anExtent, VkFormat format)
        : device(aDevice), extent(anExtent), commandPool(aDevice, graphicsQueue),
          image(aDevice, anExtent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        extent.width = anExtent.width;
        extent.height = anExtent.height;

        VK_CHECK_FAIL(createReadbackSampler(), "failed to create readback sampler");
        texture = ImGui_ImplVulkan_AddTexture(sampler, image.getView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); // @TODO: how to check for error ?

        // Transition depth image to correct layout
        VK_CHECK_FAIL(image.transitionImageLayout(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool),
                      "failed to transition layout of readback image");
        VK_CHECK_FAIL(image.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool),
                      "failed to transition layout of readback image");
    }

    VkResult ReadbackTexture::createReadbackSampler() {

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 16;
        samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        samplerInfo.mipLodBias = 0.0f;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;

        return vkCreateSampler(device.logical, &samplerInfo, nullptr, &sampler);
    }

    ImTextureID ReadbackTexture::updateTexture(VKHelper::Image &inputImage) {
        VK_CHECK_RET_NULL(image.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool));
        VK_CHECK_RET_NULL(inputImage.copyTo(image, commandPool));
        VK_CHECK_RET_NULL(image.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool));
        return texture;

        // VkImageMemoryBarrier imageMemoryBarrier = {};
        // imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        // imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        // imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        // imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        // imageMemoryBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        // imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        // imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        // imageMemoryBarrier.image = image.getImage();
        // imageMemoryBarrier.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1};

        // auto cmd = commandPool.beginSingleTimeCommands();
        // vkCmdPipelineBarrier(cmd, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
        // commandPool.endSingleTimeCommands(cmd);

        // // Get layout of the image (including row pitch)
        // VkImageSubresource subResource{};
        // subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        // VkSubresourceLayout subResourceLayout;

        // vkGetImageSubresourceLayout(device.logical, image.getImage(), &subResource, &subResourceLayout);

        // // Map image memory so we can start copying from it
        // const char *imagedata;
        // vkMapMemory(device.logical, image.getMemory(), 0, VK_WHOLE_SIZE, 0, (void **)&imagedata);
        // imagedata += subResourceLayout.offset;

        // const char *filename = "headless.ppm";
        // std::ofstream file(filename, std::ios::out | std::ios::binary);

        // // ppm header
        // file << "P6\n" << extent.width << "\n" << extent.height << "\n" << 255 << "\n";

        // // Check if source is BGR and needs swizzle
        // std::vector<VkFormat> formatsBGR = {VK_FORMAT_B8G8R8A8_SRGB, VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_B8G8R8A8_SNORM};
        // const bool colorSwizzle = (std::find(formatsBGR.begin(), formatsBGR.end(), VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());

        // // ppm binary pixel data
        // for (int32_t y = 0; y < extent.height; y++) {
        //     unsigned int *row = (unsigned int *)imagedata;
        //     for (int32_t x = 0; x < extent.width; x++) {
        //         if (colorSwizzle) {
        //             file.write((char *)row + 2, 1);
        //             file.write((char *)row + 1, 1);
        //             file.write((char *)row, 1);
        //         } else {
        //             file.write((char *)row, 3);
        //         }
        //         row++;
        //     }
        //     imagedata += subResourceLayout.rowPitch;
        // }
        // file.close();

        // // Clean up resources
        // vkUnmapMemory(device.logical, image.getMemory());
    }

    ImTextureID ReadbackTexture::getTexture() { return texture; }

    ReadbackTexture::~ReadbackTexture() {
        // @TODO: free texture ?
        vkDestroySampler(device.logical, sampler, nullptr);
    }

} // namespace Qulkan::Vulkan