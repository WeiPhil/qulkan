#include "vulkan/api/vertex_format.hpp"

namespace VKHelper {

    const std::vector<VkVertexInputAttributeDescription> VertexFormat::getAttributeDescriptions() { return createAttributeDescriptions(); }
    const VkVertexInputBindingDescription VertexFormat::getBindingDescription() { return createBindingDescription(); }

    VertexFormat::~VertexFormat() {}

} // namespace VKHelper