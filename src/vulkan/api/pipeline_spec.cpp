#include "vulkan/api/pipeline_spec.hpp"

namespace VKHelper {

    VkPipelineInputAssemblyStateCreateInfo PipelineSpec::getInputAssembly() { return createInputAssembly(); }
    std::vector<VkViewport> PipelineSpec::getViewports() { return createViewports(); }
    std::vector<VkRect2D> PipelineSpec::getScissors() { return createScissors(); }
    VkPipelineRasterizationStateCreateInfo PipelineSpec::getRasterizer() { return createRasterizer(); }
    VkPipelineMultisampleStateCreateInfo PipelineSpec::getMultisampling() { return createMultisampling(); }
    std::vector<VkPipelineColorBlendAttachmentState> PipelineSpec::getColorBlending() { return createColorBlending(); }
    std::vector<VkDescriptorSetLayout> PipelineSpec::getDescriptorSetLayouts() { return createDescriptorSetLayouts(); }
    VkPipelineDepthStencilStateCreateInfo PipelineSpec::getDepthStencil() { return createDepthStencil(); }

    PipelineSpec::~PipelineSpec() {}

} // namespace VKHelper