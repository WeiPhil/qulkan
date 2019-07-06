#ifndef __VK_HELPER_QUEUE_HPP__
#define __VK_HELPER_QUEUE_HPP__

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    struct Queue {
        VkQueue queue;
        uint32_t family;

        Queue(VkQueue queue, uint32_t family) : queue(queue), family(family){};
        Queue(const Queue &queue) : queue(queue.queue), family(queue.family){};
    };

} // namespace VKHelper

#endif //__VK_HELPER_QUEUE_HPP__