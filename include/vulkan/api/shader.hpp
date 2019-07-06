#ifndef __VK_HELPER_SHADER_HPP__
#define __VK_HELPER_SHADER_HPP__

#include "vulkan/api/device.hpp"
#include "vulkan/api/vk_helper.hpp"

namespace VKHelper {

    class Shader {

      private:
        Device device;
        std::string filename;
        VkShaderStageFlagBits stage;
        const std::string pName;

        VkShaderModule shaderModule = VK_NULL_HANDLE;

      public:
        static int readFile(const std::string &filename, std::vector<char> &data);

        Shader(Device device, const std::string &filename, VkShaderStageFlagBits stage, const std::string & = "main");

        std::optional<VkPipelineShaderStageCreateInfo> getShaderStageInfo();

        ~Shader();
    };

} // namespace VKHelper

#endif //__VK_HELPER_SHADER_HPP__