#ifndef __QULKAN_VULKAN_SIMPLE_PIPELINE_HPP__
#define __QULKAN_VULKAN_SIMPLE_PIPELINE_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/api/pipeline_spec.hpp"

namespace Qulkan::Vulkan {

    class SimplePipeline : public VKHelper::PipelineSpec {

      public:
        SimplePipeline(VkExtent2D &extent);

        ~SimplePipeline();

      private:
        VkExtent2D extent;

        virtual VkPipelineInputAssemblyStateCreateInfo createInputAssembly() const;
        virtual std::vector<VkViewport> createViewports() const;
        virtual std::vector<VkRect2D> createScissors() const;
        virtual VkPipelineRasterizationStateCreateInfo createRasterizer() const;
        virtual VkPipelineMultisampleStateCreateInfo createMultisampling() const;
        virtual std::vector<VkPipelineColorBlendAttachmentState> createColorBlending() const;
        virtual std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts() const;
        virtual VkPipelineDepthStencilStateCreateInfo createDepthStencil() const;
    };

} // namespace Qulkan::Vulkan

#endif //__QULKAN_VULKAN_SIMPLE_PIPELINE_HPP__