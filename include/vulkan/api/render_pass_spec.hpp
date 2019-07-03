#ifndef __VK_HELPER_RENDER_PASS_SPEC_HPP__
#define __VK_HELPER_RENDER_PASS_SPEC_HPP__

#include <vulkan/vulkan.h>

#include <vector>

#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class RenderPassSpec {

      public:
        std::vector<VkAttachmentDescription> getAttachments() const;
        std::vector<VkSubpassDescription> getSubpasses() const;
        std::vector<VkSubpassDependency> getDependencies() const;

        virtual ~RenderPassSpec();

      private:
        virtual std::vector<VkAttachmentDescription> createAttachments() const = 0;
        virtual std::vector<VkSubpassDescription> createSubpasses() const = 0;
        virtual std::vector<VkSubpassDependency> createDependencies() const = 0;
    };

} // namespace VKHelper

#endif //__VK_HELPER_RENDER_PASS_SPEC_HPP__