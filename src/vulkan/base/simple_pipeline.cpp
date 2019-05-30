#include "vulkan/base/simple_pipeline.hpp"

namespace Qulkan {

    namespace Vulkan {

        SimplePipeline::SimplePipeline(VkExtent2D &extent) : extent(extent) {}

        const VkPipelineInputAssemblyStateCreateInfo SimplePipeline::createInputAssembly() {
            VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;
            return inputAssembly;
        }

        const std::vector<VkViewport> SimplePipeline::createViewports() {
            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = extent.width;
            viewport.height = extent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            return std::vector<VkViewport>{viewport};
        }

        const std::vector<VkRect2D> SimplePipeline::createScissors() {
            VkRect2D scissor = {};
            scissor.offset = {0, 0};
            scissor.extent = extent;
            return std::vector<VkRect2D>{scissor};
        }

        const VkPipelineRasterizationStateCreateInfo SimplePipeline::createRasterizer() {
            VkPipelineRasterizationStateCreateInfo rasterizer = {};
            rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
            rasterizer.depthClampEnable = VK_FALSE;
            rasterizer.rasterizerDiscardEnable = VK_FALSE;
            rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
            rasterizer.lineWidth = 1.0f;
            rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
            rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
            rasterizer.depthBiasEnable = VK_FALSE;
            return rasterizer;
        }

        const VkPipelineMultisampleStateCreateInfo SimplePipeline::createMultisampling() {
            VkPipelineMultisampleStateCreateInfo multisampling = {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            return multisampling;
        }

        const std::vector<VkPipelineColorBlendAttachmentState> SimplePipeline::createColorBlending() {
            VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            return std::vector<VkPipelineColorBlendAttachmentState>{colorBlendAttachment};
        }

        const std::vector<VkDescriptorSetLayout> SimplePipeline::createDescriptorSetLayouts() { return std::vector<VkDescriptorSetLayout>(); }

        const VkPipelineDepthStencilStateCreateInfo SimplePipeline::createDepthStencil() {
            VkPipelineDepthStencilStateCreateInfo depthStencil = {};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f;
            depthStencil.maxDepthBounds = 1.0f;
            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.front = {};
            depthStencil.back = {};
            return depthStencil;
        }

        SimplePipeline::~SimplePipeline() {}

    } // namespace Vulkan
} // namespace Qulkan
