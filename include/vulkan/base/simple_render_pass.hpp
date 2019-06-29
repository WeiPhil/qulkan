#ifndef __QULKAN_VULKAN_SIMPLE_RENDER_PASS_HPP__
#define __QULKAN_VULKAN_SIMPLE_RENDER_PASS_HPP__

#include "vulkan/api/device.hpp"
#include "vulkan/api/render_pass_spec.hpp"

namespace Qulkan::Vulkan {

    class SimpleRenderPass : VKHelper::RenderPassSpec {

      public:
        SimpleRenderPass(VKHelper::Device aDevice, VkFormat aFormat);

        virtual ~SimpleRenderPass();

      private:
        const VKHelper::Device device;
        const VkFormat format;

        virtual std::vector<VkAttachmentDescription> createAttachments();
        virtual std::vector<VkSubpassDescription> createSubpasses();
        virtual std::vector<VkSubpassDependency> createDependencies();
    };

} // namespace Qulkan::Vulkan

#endif //__QULKAN_VULKAN_SIMPLE_RENDER_PASS_HPP__