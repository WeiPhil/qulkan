#ifndef __VK_HELPER_PIPELINE_INFO_HPP__
#define __VK_HELPER_PIPELINE_INFO_HPP__

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    // @TODO: doesn't handle its own destruction. It should do it
    struct PipelineInfo {
        const VkPipeline pipeline;
        const VkPipelineLayout layout;

        PipelineInfo(VkPipeline pipeline, VkPipelineLayout layout) : pipeline(pipeline), layout(layout) {}
        PipelineInfo(const PipelineInfo &info) : pipeline(info.pipeline), layout(info.layout) {}
    };

} // namespace VKHelper

#endif //__VK_HELPER_PIPELINE_INFO_HPP__