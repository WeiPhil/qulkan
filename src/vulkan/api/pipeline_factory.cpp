#include "vulkan/api/pipeline_factory.hpp"

namespace VKHelper {

    PipelineFactory::PipelineFactory(VKHelper::Device &device) : device(device) {}

    void PipelineFactory::addPipelineToSet(VkPipeline pipeline, VkPipelineLayout layout) {
        createdPipelines.insert(PipelineInfo {pipeline, layout});
    }

    void PipelineFactory::destroyPipeline(VkPipeline pipeline) {
        ASSERT_MSG(createdPipelines.erase(PipelineInfo {pipeline, VK_NULL_HANDLE}) == 1, "attempting to delete non-existent pipeline");
    }

    PipelineFactory::~PipelineFactory() {
        for (const auto& pipelineInfo: createdPipelines) {
            vkDestroyPipeline(device.logical, pipelineInfo.pipeline, nullptr);
            vkDestroyPipelineLayout(device.logical, pipelineInfo.layout, nullptr);
        }
    }

} // namespace VKHelper