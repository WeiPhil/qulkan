#include "vulkan/vk_helper.hpp"

#include <fstream>

bool VKHelper::hasStencilComponent(VkFormat format) { return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT; }