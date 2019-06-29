#include "vulkan/readback_texture.hpp"

#include "imgui/imgui_impl_vulkan.h"

namespace Qulkan::Vulkan {

    ReadbackTexture::ReadbackTexture(VKHelper::Device aDevice, VkExtent2D anExtent, VkFormat format)
        : device(aDevice), image(device, extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_ASPECT_COLOR_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {

        extent.width = anExtent.width;
        extent.height = anExtent.height;

        VK_CHECK_FAIL(createReadbackSampler(), "failed to create readback sampler");
        texture = ImGui_ImplVulkan_AddTexture(sampler, image.getView(), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL); // @TODO: how to check for error ? 
    }

    VkResult ReadbackTexture::createReadbackSampler() {

        VkSamplerCreateInfo samplerInfo = {};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_LINEAR;
        samplerInfo.minFilter = VK_FILTER_LINEAR;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        samplerInfo.anisotropyEnable = VK_TRUE;
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

    ImTextureID ReadbackTexture::updateTexture(VKHelper::Image inputImage, VKHelper::CommandPool commandPool) {
        VK_CHECK_RET_NULL(image.transitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, commandPool));
        VK_CHECK_RET_NULL(inputImage.copyTo(image, commandPool));
        VK_CHECK_RET_NULL(image.transitionImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, commandPool));
        return texture;
    }

    ImTextureID ReadbackTexture::getTexture() {
        return texture;
    }

    ReadbackTexture::~ReadbackTexture() {
        // @TODO: free texture ?
        vkDestroySampler(device.logical, sampler, nullptr);
    }

} // namespace Qulkan::Vulkan