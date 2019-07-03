#ifndef __VULKAN_QULKAN_SIMPLE_VERTEX_FORMAT_HPP__
#define __VULKAN_QULKAN_SIMPLE_VERTEX_FORMAT_HPP__

#include <glm/glm.hpp>

#include "vulkan/api/vertex_format.hpp"

namespace Qulkan::Vulkan {

    class ColoredVertex final : public VKHelper::VertexFormat {

      public:
        ColoredVertex(); // Used to get access to the attribute and binding descriptions
        ColoredVertex(const glm::vec3 &aPos, const glm::vec3 &aColor);

        virtual ~ColoredVertex();

      private:
        glm::vec3 pos;
        glm::vec3 color;

        virtual std::vector<VkVertexInputAttributeDescription> createAttributeDescriptions() const;
        virtual VkVertexInputBindingDescription createBindingDescription() const;
    };

} // namespace Qulkan::Vulkan

#endif //__VULKAN_QULKAN_SIMPLE_VERTEX_FORMAT_HPP__