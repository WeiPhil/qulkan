#include "vulkan/base/simple_vertex_format.hpp"

namespace Qulkan::Vulkan {

    ColoredVertex::ColoredVertex() {}
    ColoredVertex::ColoredVertex(const glm::vec3 &aPos, const glm::vec3 &aColor) : pos(aPos), color(aColor) {}

    std::vector<VkVertexInputAttributeDescription> ColoredVertex::createAttributeDescriptions() const {

        // Position
        VkVertexInputAttributeDescription posAttachment;
        posAttachment.binding = 0;
        posAttachment.location = 0;
        posAttachment.format = VK_FORMAT_R32G32B32_SFLOAT;
        posAttachment.offset = offsetof(ColoredVertex, pos);

        // Color
        VkVertexInputAttributeDescription colorAttachment;
        colorAttachment.binding = 0;
        colorAttachment.location = 1;
        colorAttachment.format = VK_FORMAT_R32G32B32_SFLOAT;
        colorAttachment.offset = offsetof(ColoredVertex, color);

        return std::vector<VkVertexInputAttributeDescription>{posAttachment, colorAttachment};
    }

    VkVertexInputBindingDescription ColoredVertex::createBindingDescription() const {
        VkVertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(ColoredVertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    ColoredVertex::~ColoredVertex(){};

} // namespace Qulkan::Vulkan
