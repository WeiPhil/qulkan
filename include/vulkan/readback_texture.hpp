#ifndef __READBACK_TEXTURE_HPP__
#define __READBACK_TEXTURE_HPP__

#include "imgui.h"

#include "vulkan/api/device.hpp"
#include "vulkan/api/image.hpp"

namespace Qulkan::Vulkan {

    class ReadbackTexture {

      public:
        ReadbackTexture(VKHelper::Device aDevice, VkExtent2D anExtent, VkFormat format);

        ImTextureID updateTexture(VKHelper::Image inputImage, VKHelper::CommandPool commandPool);

        ImTextureID getTexture();

        virtual ~ReadbackTexture();

      private:
        const VKHelper::Device device;
        VkExtent2D extent;

        ImTextureID texture;
        VKHelper::Image image;
        VkSampler sampler = VK_NULL_HANDLE;

        virtual VkResult createReadbackSampler();
    };

} // namespace Qulkan::Vulkan

#endif //__READBACK_TEXTURE_HPP__