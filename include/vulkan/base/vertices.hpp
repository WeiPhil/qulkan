#ifndef __BASE_VERTICES_HPP__
#define __BASE_VERTICES_HPP__

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace Base {

    template <class VertexFormat> class Vertices {
      private:
        std::vector<VertexFormat> data;

      public:
        const std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() { return VertexFormat::getAttributeDescriptions(); };

        const VkVertexInputBindingDescription getBindingDescription() {
            VkVertexInputBindingDescription bindingDescription = {};
            bindingDescription.binding = 0;
            bindingDescription.stride = sizeof(VertexFormat);
            bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
            return bindingDescription;
        };

        // @TODO Add methods to manipulate the data vector
    };

    class NullVertex {
      public:
        const static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() { return std::vector<VkVertexInputAttributeDescription>(); }
    };

    class ColoredVertex {
      public:
        glm::vec3 pos;
        glm::vec3 color;

        const static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {

            VkVertexInputAttributeDescription descriptions[2];

            // Position
            VkVertexInputAttributeDescription posDescription;
            posDescription.binding = 0;
            posDescription.location = 0;
            posDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
            posDescription.offset = offsetof(ColoredVertex, pos);

            // Color
            VkVertexInputAttributeDescription colorDescription;
            colorDescription.binding = 0;
            colorDescription.location = 1;
            colorDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
            colorDescription.offset = offsetof(ColoredVertex, color);

            return std::vector<VkVertexInputAttributeDescription>{posDescription, colorDescription};
        }
    };

} // namespace Base

#endif //__BASE_VERTICES_HPP__