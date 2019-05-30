#ifndef __VK_HELPER_PIPELINE_SPEC_HPP__
#define __VK_HELPER_PIPELINE_SPEC_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class PipelineSpec {

      public:
        const VkPipelineInputAssemblyStateCreateInfo getInputAssembly();
        const std::vector<VkViewport> getViewports();
        const std::vector<VkRect2D> getScissors();
        const VkPipelineRasterizationStateCreateInfo getRasterizer();
        const VkPipelineMultisampleStateCreateInfo getMultisampling();
        const std::vector<VkPipelineColorBlendAttachmentState> getColorBlending();
        const std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();
        const VkPipelineDepthStencilStateCreateInfo getDepthStencil();

        virtual ~PipelineSpec();

      private:
        virtual const VkPipelineInputAssemblyStateCreateInfo createInputAssembly() = 0;
        virtual const std::vector<VkViewport> createViewports() = 0;
        virtual const std::vector<VkRect2D> createScissors() = 0;
        virtual const VkPipelineRasterizationStateCreateInfo createRasterizer() = 0;
        virtual const VkPipelineMultisampleStateCreateInfo createMultisampling() = 0;
        virtual const std::vector<VkPipelineColorBlendAttachmentState> createColorBlending() = 0;
        virtual const std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts() = 0;
        virtual const VkPipelineDepthStencilStateCreateInfo createDepthStencil() = 0;
    };

} // namespace VKHelper

#endif //__VK_HELPER_PIPELINE_SPEC_HPP__