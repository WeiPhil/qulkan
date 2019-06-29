#ifndef __VK_HELPER_PIPELINE_SPEC_HPP__
#define __VK_HELPER_PIPELINE_SPEC_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class PipelineSpec {

      public:
        VkPipelineInputAssemblyStateCreateInfo getInputAssembly();
        std::vector<VkViewport> getViewports();
        std::vector<VkRect2D> getScissors();
        VkPipelineRasterizationStateCreateInfo getRasterizer();
        VkPipelineMultisampleStateCreateInfo getMultisampling();
        std::vector<VkPipelineColorBlendAttachmentState> getColorBlending();
        std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts();
        VkPipelineDepthStencilStateCreateInfo getDepthStencil();

        virtual ~PipelineSpec();

      private:
        virtual VkPipelineInputAssemblyStateCreateInfo createInputAssembly() = 0;
        virtual std::vector<VkViewport> createViewports() = 0;
        virtual std::vector<VkRect2D> createScissors() = 0;
        virtual VkPipelineRasterizationStateCreateInfo createRasterizer() = 0;
        virtual VkPipelineMultisampleStateCreateInfo createMultisampling() = 0;
        virtual std::vector<VkPipelineColorBlendAttachmentState> createColorBlending() = 0;
        virtual std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts() = 0;
        virtual VkPipelineDepthStencilStateCreateInfo createDepthStencil() = 0;
    };

} // namespace VKHelper

#endif //__VK_HELPER_PIPELINE_SPEC_HPP__