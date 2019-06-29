#ifndef __VK_HELPER_DEVICE_HPP__
#define __VK_HELPER_DEVICE_HPP__

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    struct Device {
        const VkPhysicalDevice physical;
        const VkDevice logical;

        Device(VkPhysicalDevice physicalDevice, VkDevice device);
        Device(const Device &device);

        const std::optional<uint32_t> findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

        const VkFormat findSupportedFormat(const std::vector<VkFormat> &candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

        const VkFormat findDepthFormat();
    };

} // namespace VKHelper

#endif //__VK_HELPER_DEVICE_HPP__