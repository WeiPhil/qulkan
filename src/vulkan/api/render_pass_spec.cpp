#include "vulkan/api/render_pass_spec.hpp"

namespace VKHelper {

    std::vector<VkAttachmentDescription> RenderPassSpec::getAttachments() const { return createAttachments(); }
    std::vector<VkSubpassDescription> RenderPassSpec::getSubpasses() const { return createSubpasses(); }
    std::vector<VkSubpassDependency> RenderPassSpec::getDependencies() const { return createDependencies(); }

    RenderPassSpec::~RenderPassSpec() {}

} // namespace VKHelper