#ifndef __BASE_PIPELINE_HPP__
#define __BASE_PIPELINE_HPP__

#include <vulkan/libspirv.h>
#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/base/vertex.hpp"
#include "vulkan/vk_helper.hpp"

namespace Base {

    template <class VertexFormat> class Pipeline {

      private:
        VKHelper::Device device;
        VkExtent2D extent;

        VkPipeline pipeline = VK_NULL_HANDLE;
        VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

      public:
        Pipeline(VKHelper::Device &device, VkExtent2D &extent) : device(device), extent(extent){};

        virtual VkPipelineInputAssemblyStateCreateInfo createInputAssembly() {
            VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
            inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
            inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            inputAssembly.primitiveRestartEnable = VK_FALSE;
            return inputAssembly;
        }

        virtual std::vector<VkViewport> createViewports() {
            VkViewport viewport = {};
            viewport.x = 0.0f;
            viewport.y = 0.0f;
            viewport.width = extent.width;
            viewport.height = extent.height;
            viewport.minDepth = 0.0f;
            viewport.maxDepth = 1.0f;
            return std::vector<VkViewport>{viewport};
        }

        virtual std::vector<VkRect2D> createScissors() {
            VkRect2D scissor = {};
            scissor.offset = {0, 0};
            scissor.extent = extent;
            return std::vector<VkRect2D>{scissor};
        }

        virtual VkPipelineRasterizationStateCreateInfo createRasterizer() {
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

        virtual VkPipelineMultisampleStateCreateInfo createMultisampling() {
            VkPipelineMultisampleStateCreateInfo multisampling = {};
            multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
            multisampling.sampleShadingEnable = VK_FALSE;
            multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
            return multisampling;
        }

        virtual std::vector<VkPipelineColorBlendAttachmentState> createColorBlending() {
            VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
            colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
            colorBlendAttachment.blendEnable = VK_FALSE;
            return std::vector<VkPipelineColorBlendAttachmentState>{colorBlendAttachment};
        }

        virtual std::vector<VkDescriptorSetLayout> createDescriptorSetLayouts() {
            return std::vector<VkDescriptorSetLayout>();
        }

        virtual VkPipelineDepthStencilStateCreateInfo createDepthStencil() {
            VkPipelineDepthStencilStateCreateInfo depthStencil = {};
            depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
            depthStencil.depthTestEnable = VK_TRUE;
            depthStencil.depthWriteEnable = VK_TRUE;
            depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
            depthStencil.depthBoundsTestEnable = VK_FALSE;
            depthStencil.minDepthBounds = 0.0f; // Optional
            depthStencil.maxDepthBounds = 1.0f; // Optional
            depthStencil.stencilTestEnable = VK_FALSE;
            depthStencil.front = {}; // Optional
            depthStencil.back = {};  // Optional
            return depthStencil;
        }

        virtual VkPipeline createGraphicsPipeline(Vertices<VertexFormat> &vertices, VkRenderPass renderPass) {

            // @TODO where do we get the shaders' path from ?

            // Load 2 dummy shaders (vertex + fragment). ShaderModules freed automatically when these objects go out of scope
            VKHelper::Shader vertexShader{device, "../data/shaders/vk_vert.spv", VK_SHADER_STAGE_VERTEX_BIT};
            VKHelper::Shader fragmentShader{device, "../data/shaders/vk_frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT};

            // Create shader stages info
            std::optional<VkPipelineShaderStageCreateInfo> vertexShaderInfo = vertexShaderInfo;
            std::optional<VkPipelineShaderStageCreateInfo> fragmentShaderInfo = fragmentShaderInfo;
            if (!vertexShaderInfo || !fragmentShaderInfo) {
                return VK_NULL_HANDLE;
            }
            std::vector<VkPipelineShaderStageCreateInfo> shaderStages{vertexShaderInfo, fragmentShaderInfo};

            auto bindingDescription = vertices.getBindingDescription();
            auto attributeDescriptions = vertices.getAttributeDescriptions();

            // Vertex input
            VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
            vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
            vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
            vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

            // Input assembly
            VkPipelineInputAssemblyStateCreateInfo inputAssembly = createInputAssembly();

            // Viewports
            std::vector<VkViewport> viewports = createViewports();

            // Scissors
            std::vector<VkRect2D> scissors = createScissors();

            // Viewport state
            VkPipelineViewportStateCreateInfo viewportState = {};
            viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
            viewportState.viewportCount = static_cast<uint32_t>(viewports.size());
            viewportState.pViewports = viewports.data();
            viewportState.scissorCount = static_cast<uint32_t>(scissors.size());
            viewportState.pScissors = scissors.data();

            // Rasterizer
            VkPipelineRasterizationStateCreateInfo rasterizer = createRasterizer();

            // Multisampling
            VkPipelineMultisampleStateCreateInfo multisampling = createMultisampling();

            // Color blending
            std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments = createColorBlending();

            // Color blend state (depends on colorBlendAttachments)
            VkPipelineColorBlendStateCreateInfo colorBlending = {};
            colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
            colorBlending.logicOpEnable = VK_FALSE;
            colorBlending.attachmentCount = static_cast<uint32_t>(colorBlendAttachments.size());
            colorBlending.pAttachments = colorBlendAttachments.data();

            // Descriptor set layouts
            std::vector<VkDescriptorSetLayout> descriptorSetLayouts = createDescriptorSetLayouts();

            // Pipeline layout (depends on descriptorSetLayouts)
            VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
            pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
            pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
            pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

            VkResult ret;
            if ((ret = vkCreatePipelineLayout(device.logical, &pipelineLayoutInfo, nullptr, &pipelineLayout)) != VK_SUCCESS) {
                return VK_NULL_HANDLE;
            }

            // Depth/Stencil
            VkPipelineDepthStencilStateCreateInfo depthStencil = createDepthStencil();

            // Final structure (depends on all of the above + render pass)
            VkGraphicsPipelineCreateInfo pipelineInfo = {};
            pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
            pipelineInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
            pipelineInfo.pStages = shaderStages.data();
            pipelineInfo.pVertexInputState = &vertexInputInfo;
            pipelineInfo.pInputAssemblyState = &inputAssembly;
            pipelineInfo.pViewportState = &viewportState;
            pipelineInfo.pRasterizationState = &rasterizer;
            pipelineInfo.pMultisampleState = &multisampling;
            pipelineInfo.pDepthStencilState = &depthStencil;
            pipelineInfo.pColorBlendState = &colorBlending;
            pipelineInfo.pDynamicState = nullptr;
            pipelineInfo.layout = pipelineLayout;
            pipelineInfo.renderPass = renderPass;
            pipelineInfo.subpass = 0;
            pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
            pipelineInfo.basePipelineIndex = -1;

            // Create the graphics pipeline
            if ((ret = vkCreateGraphicsPipelines(device.logical, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline)) != VK_SUCCESS) {
                return VK_NULL_HANDLE;
            }

            return pipeline;
        }

        ~Pipeline() {
            if (pipeline) {
                vkDestroyPipeline(device.logical, pipeline, nullptr);
            }
            if (pipelineLayout) {
                vkDestroyPipelineLayout(device.logical, pipelineLayout, nullptr);
            }
        }
    };

} // namespace Base

#endif //__BASE_PIPELINE_HPP__