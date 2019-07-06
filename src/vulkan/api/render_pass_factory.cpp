#include "vulkan/api/render_pass_factory.hpp"

namespace VKHelper {

    RenderPassFactory::RenderPassFactory(VKHelper::Device &device) : device(device) {}

    void RenderPassFactory::addRenderPassToSet(VkRenderPass renderPass) {
        createdRenderPasses.insert(renderPass);
    }

    void RenderPassFactory::destroyRenderPass(VkRenderPass renderPass) {
        ASSERT_MSG(createdRenderPasses.erase(renderPass) == 1, "attempting to delete non-existent render pass");
    }

    RenderPassFactory::~RenderPassFactory() {
        for (const auto& renderPass: createdRenderPasses) {
            vkDestroyRenderPass(device.logical, renderPass, nullptr);
        }
    }

} // namespace VKHelper