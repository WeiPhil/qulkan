#include "vulkan/api/pipeline_spec.hpp"

namespace VKHelper {

    const VkPipelineInputAssemblyStateCreateInfo PipelineSpec::getInputAssembly() { return createInputAssembly(); }
    const std::vector<VkViewport> PipelineSpec::getViewports() { return createViewports(); }
    const std::vector<VkRect2D> PipelineSpec::getScissors() { return createScissors(); }
    const VkPipelineRasterizationStateCreateInfo PipelineSpec::getRasterizer() { return createRasterizer(); }
    const VkPipelineMultisampleStateCreateInfo PipelineSpec::getMultisampling() { return createMultisampling(); }
    const std::vector<VkPipelineColorBlendAttachmentState> PipelineSpec::getColorBlending() { return createColorBlending(); }
    const std::vector<VkDescriptorSetLayout> PipelineSpec::getDescriptorSetLayouts() { return createDescriptorSetLayouts(); }
    const VkPipelineDepthStencilStateCreateInfo PipelineSpec::getDepthStencil() { return createDepthStencil(); }

    PipelineSpec::~PipelineSpec() {}

} // namespace VKHelper