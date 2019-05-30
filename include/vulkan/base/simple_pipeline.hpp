#ifndef __QULKAN_VULKAN_SIMPLE_PIPELINE_HPP__
#define __QULKAN_VULKAN_SIMPLE_PIPELINE_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/api/pipeline_spec.hpp"

namespace Qulkan {

    namespace Vulkan {

        class SimplePipeline : VKHelper::PipelineSpec {

          public:
            SimplePipeline(VkExtent2D &extent);

            ~SimplePipeline();

          private:
            VkExtent2D extent;

            virtual const VkPipelineInputAssemblyStateCreateInfo createInputAssembly();
            virtual const std::vector<VkViewport> createViewports();
            virtual const std::vector<VkRect2D> createScissors();
            virtual const VkPipelineRasterizationStateCreateInfo createRasterizer();
            virtual const VkPipelineMultisampleStateCreateInfo createMultisampling();
            virtual const std::vector<VkPipelineColorBlendAttachmentState> createColorBlending();
            virtual const std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts();
            virtual const VkPipelineDepthStencilStateCreateInfo createDepthStencil();
        };

    } // namespace Vulkan

} // namespace Qulkan

#endif //__QULKAN_VULKAN_SIMPLE_PIPELINE_HPP__