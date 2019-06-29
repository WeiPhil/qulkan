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

            virtual VkPipelineInputAssemblyStateCreateInfo createInputAssembly();
            virtual std::vector<VkViewport> createViewports();
            virtual std::vector<VkRect2D> createScissors();
            virtual VkPipelineRasterizationStateCreateInfo createRasterizer();
            virtual VkPipelineMultisampleStateCreateInfo createMultisampling();
            virtual std::vector<VkPipelineColorBlendAttachmentState> createColorBlending();
            virtual std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts();
            virtual VkPipelineDepthStencilStateCreateInfo createDepthStencil();
        };

    } // namespace Vulkan

} // namespace Qulkan

#endif //__QULKAN_VULKAN_SIMPLE_PIPELINE_HPP__