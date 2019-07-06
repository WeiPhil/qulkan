#ifndef __VK_HELPER_HPP__
#define __VK_HELPER_HPP__

#include <assert.h>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#define ASSERT_MSG(b, err_str)                                                                                                                                 \
    {                                                                                                                                                          \
        if (!(b)) {                                                                                                                                            \
            std::cout << "[FATAL]: " << err_str << " in " << __FILE__ << " at line " << __LINE__ << std::endl;                                                 \
            assert(b);                                                                                                                                         \
        }                                                                                                                                                      \
    }

#define VK_CHECK_NULL(handle)                                                                                                                                  \
    {                                                                                                                                                          \
        if ((handle) != VK_NULL_HANDLE) {                                                                                                                      \
            std::cout << "[FATAL]: " << #handle << " isn't VK_NULL_HANDLE"                                                                                     \
                      << " in " << __FILE__ << " at line " << __LINE__ << std::endl;                                                                           \
            assert(false);                                                                                                                                     \
        }                                                                                                                                                      \
    }

#define VK_CHECK_NOT_NULL(handle)                                                                                                                              \
    {                                                                                                                                                          \
        if ((handle) == VK_NULL_HANDLE) {                                                                                                                      \
            std::cout << "[FATAL]: " << #handle << " is VK_NULL_HANDLE"                                                                                        \
                      << " in " << __FILE__ << " at line " << __LINE__ << std::endl;                                                                           \
            assert(false);                                                                                                                                     \
        }                                                                                                                                                      \
    }

#define VK_CHECK_FAIL(b, err_str)                                                                                                                              \
    {                                                                                                                                                          \
        VkResult res = (b);                                                                                                                                    \
        if (res != VK_SUCCESS) {                                                                                                                               \
            std::cout << "[FATAL]: " << err_str << " in " << __FILE__ << " at line " << __LINE__ << std::endl;                                                 \
            assert(false);                                                                                                                                     \
        }                                                                                                                                                      \
    }

#define VK_CHECK_RET(b)                                                                                                                                        \
    {                                                                                                                                                          \
        VkResult res = (b);                                                                                                                                    \
        if (res != VK_SUCCESS) {                                                                                                                               \
            return res;                                                                                                                                        \
        }                                                                                                                                                      \
    }

#define VK_CHECK_RET_NULL(b)                                                                                                                                   \
    {                                                                                                                                                          \
        VkResult res = (b);                                                                                                                                    \
        if (res != VK_SUCCESS) {                                                                                                                               \
            return VK_NULL_HANDLE;                                                                                                                             \
        }                                                                                                                                                      \
    }

namespace VKHelper {

    bool hasStencilComponent(VkFormat format);

} // namespace VKHelper

#endif //__VK_HELPER_HPP__