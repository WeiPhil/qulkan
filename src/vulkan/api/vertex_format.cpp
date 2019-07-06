#include "vulkan/api/vertex_format.hpp"

namespace VKHelper {

    std::vector<VkVertexInputAttributeDescription> VertexFormat::getAttributeDescriptions() const { return createAttributeDescriptions(); }
    VkVertexInputBindingDescription VertexFormat::getBindingDescription() const { return createBindingDescription(); }
    
    VertexFormat::~VertexFormat() {}

} // namespace VKHelper