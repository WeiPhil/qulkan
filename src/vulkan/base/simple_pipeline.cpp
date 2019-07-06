#include "vulkan/base/simple_pipeline.hpp"

namespace Qulkan::Vulkan {

    SimplePipeline::SimplePipeline(VkExtent2D &extent) : extent(extent) {}

    VkPipelineInputAssemblyStateCreateInfo SimplePipeline::createInputAssembly() const {
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        return inputAssembly;
    }

    std::vector<VkViewport> SimplePipeline::createViewports() const {
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(extent.width);
        viewport.height = static_cast<float>(extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        return std::vector<VkViewport>{viewport};
    }

    std::vector<VkRect2D> SimplePipeline::createScissors() const {
        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = extent;
        return std::vector<VkRect2D>{scissor};
    }

    VkPipelineRasterizationStateCreateInfo SimplePipeline::createRasterizer() const {
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;
        return rasterizer;
    }

    VkPipelineMultisampleStateCreateInfo SimplePipeline::createMultisampling() const {
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        return multisampling;
    }

    std::vector<VkPipelineColorBlendAttachmentState> SimplePipeline::createColorBlending() const {
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        return std::vector<VkPipelineColorBlendAttachmentState>{colorBlendAttachment};
    }

    std::vector<VkDescriptorSetLayout> SimplePipeline::createDescriptorSetLayouts() const { return std::vector<VkDescriptorSetLayout>(); }

    VkPipelineDepthStencilStateCreateInfo SimplePipeline::createDepthStencil() const {
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

} // namespace Qulkan::Vulkan
