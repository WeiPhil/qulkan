#ifndef __VK_HELPER_RENDER_PASS_SPEC_HPP__
#define __VK_HELPER_RENDER_PASS_SPEC_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class RenderPassSpec {

      public:
        std::vector<VkAttachmentDescription> getAttachments();
        std::vector<VkSubpassDescription> getSubpasses();
        std::vector<VkSubpassDependency> getDependencies();

        virtual ~RenderPassSpec();

      private:
        virtual std::vector<VkAttachmentDescription> createAttachments() = 0;
        virtual std::vector<VkSubpassDescription> createSubpasses() = 0;
        virtual std::vector<VkSubpassDependency> createDependencies() = 0;
    };

} // namespace VKHelper

#endif //__VK_HELPER_RENDER_PASS_SPEC_HPP__