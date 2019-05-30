#ifndef __BASE_VERTICES_HPP__
#define __BASE_VERTICES_HPP__

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace VKHelper {

    class VertexFormat {

        public:
            const std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            const VkVertexInputBindingDescription getBindingDescription();

            virtual ~VertexFormat();

        private:
            virtual const std::vector<VkVertexInputAttributeDescription> createAttributeDescriptions() = 0;
            virtual const VkVertexInputBindingDescription createBindingDescription() = 0;
    };

} // namespace Base

#endif //__BASE_VERTICES_HPP__