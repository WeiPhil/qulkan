#ifndef __VK_HELPER_FENCE_HPP__
#define __VK_HELPER_FENCE_HPP__

#include "vulkan/api/device.hpp"
#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class Fence {
      private:
        Device device;
        VkFence fence = VK_NULL_HANDLE;

      public:
        Fence(Device device);

        const VkFence getFence();

        ~Fence();
    };

} // namespace VKHelper

#endif //__VK_HELPER_FENCE_HPP__