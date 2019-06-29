#include "vulkan/api/render_pass_spec.hpp"

namespace VKHelper {

    std::vector<VkAttachmentDescription> RenderPassSpec::getAttachments() { return createAttachments(); }
    std::vector<VkSubpassDescription> RenderPassSpec::getSubpasses() { return createSubpasses(); }
    std::vector<VkSubpassDependency> RenderPassSpec::getDependencies() { return createDependencies(); }

    RenderPassSpec::~RenderPassSpec() {}

} // namespace VKHelper