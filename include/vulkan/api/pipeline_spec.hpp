#ifndef __VK_HELPER_PIPELINE_SPEC_HPP__
#define __VK_HELPER_PIPELINE_SPEC_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class PipelineSpec {

      public:
        VkPipelineInputAssemblyStateCreateInfo getInputAssembly() const;
        std::vector<VkViewport> getViewports() const;
        std::vector<VkRect2D> getScissors() const;
        VkPipelineRasterizationStateCreateInfo getRasterizer() const;
        VkPipelineMultisampleStateCreateInfo getMultisampling() const;
        std::vector<VkPipelineColorBlendAttachmentState> getColorBlending() const;
        std::vector<VkDescriptorSetLayout> getDescriptorSetLayouts() const;
        VkPipelineDepthStencilStateCreateInfo getDepthStencil() const;

        virtual ~PipelineSpec();

      private:
        virtual VkPipelineInputAssemblyStateCreateInfo createInputAssembly() const = 0;
        virtual std::vector<VkViewport> createViewports() const = 0;
        virtual std::vector<VkRect2D> createScissors() const = 0;
        virtual VkPipelineRasterizationStateCreateInfo createRasterizer() const = 0;
        virtual VkPipelineMultisampleStateCreateInfo createMultisampling() const = 0;
        virtual std::vector<VkPipelineColorBlendAttachmentState> createColorBlending() const = 0;
        virtual std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts() const = 0;
        virtual VkPipelineDepthStencilStateCreateInfo createDepthStencil() const = 0;
    };

} // namespace VKHelper

#endif //__VK_HELPER_PIPELINE_SPEC_HPP__