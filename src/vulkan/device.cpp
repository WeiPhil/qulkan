#include "vulkan/vk_helper.hpp"

#include <fstream>
#include <iostream>
#include <vector>

namespace VKHelper {

    Device::Device(VkPhysicalDevice physicalDevice, VkDevice device) : physical(physicalDevice), logical(device){}
    Device::Device(const Device &device) : physical(device.physical), logical(device.logical){}

    const std::optional<uint32_t> Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

        // Query physical device for available types of memory
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physical, &memProperties);

        // Check each type of memory
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        // No suitable type found
        return {};
    }

    // @TODO define custom error codes
    const int Device::readFile(const std::string &filename, std::vector<char> &data) {
        // Open the file in read binary mode
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()) {
            return 1;
        }

        // Store the file's content in the data buffer
        size_t fileSize = (size_t)file.tellg();
        data.resize(fileSize);
        file.seekg(0);
        file.read(data.data(), fileSize);

        if (data.size() != fileSize) {
            return 2;
        }

        // Close the file and return
        file.close();
        return 0;
    }

    const VkResult Device::createShaderModule(const std::vector<char> &code, VkShaderModule &shaderModule) {

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());

        VK_CHECK_RET(vkCreateShaderModule(logical, &createInfo, nullptr, &shaderModule));
        return VK_SUCCESS;
    }

    const VkResult Device::readShader(const std::string &filename, VkShaderModule &shaderModule) {
        std::vector<char> data;
        int ret;
        if ((ret = readFile(filename, data))) {
            std::cout << "failed to read shader " << filename << ": readfile() failed with error code " << ret << std::endl;
            return VK_RESULT_MAX_ENUM;
        }
        return createShaderModule(data, shaderModule);
    }

    const VkFormat Device::findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {

        // Check each candidate format for the necessary features
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(physical, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        ASSERT_MSG(false, "failed to find supported format");
    }

    const VkFormat Device::findDepthFormat() {
        return findSupportedFormat({VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT}, VK_IMAGE_TILING_OPTIMAL,
                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    }

} // namespace VKHelper
