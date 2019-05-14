#include "vulkan/vk_helper.hpp"

#include <optional>
#include <string.h>

namespace VKHelper {

    static VkResult createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer &buffer) {

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK_RET(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));
        return VK_SUCCESS;
    }

    Buffer::Buffer(Device device, VkDeviceSize size, VkBufferUsageFlags usage) : device(device), bufferSize(size), usage(usage) {
        VK_CHECK_FAIL(createBuffer(device.logical, size, usage, buffer), "buffer creation failed");
    }

    Buffer::Buffer(Device device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
        : device(device), bufferSize(size), usage(usage) {
        VK_CHECK_FAIL(createBuffer(device.logical, size, usage, buffer), "buffer creation failed");
        VK_CHECK_FAIL(bind(properties), "buffer bind failed");
    }

    VkResult Buffer::bind(VkMemoryPropertyFlags properties) {

        VK_CHECK_NULL(memory);

        memProperties = properties;
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device.logical, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        std::optional memType = findMemoryType(device.physical, memRequirements.memoryTypeBits, properties);
        if (memType) {
            allocInfo.memoryTypeIndex = *memType;
        } else {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        VK_CHECK_RET(vkAllocateMemory(device.logical, &allocInfo, nullptr, &memory));
        VK_CHECK_RET(vkBindBufferMemory(device.logical, buffer, memory, 0));

        return VK_SUCCESS;
    }

    VkResult Buffer::mapAndCopy(void *dataToCopy, size_t size) {

        VK_CHECK_NOT_NULL(memory);
        ASSERT_MSG(memProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "memory is not mappable");

        void *data;
        VK_CHECK_RET(vkMapMemory(device.logical, memory, 0, bufferSize, 0, &data));
        memcpy(data, dataToCopy, size);
        vkUnmapMemory(device.logical, memory);

        return VK_SUCCESS;
    }

    Buffer::~Buffer() {
        vkDestroyBuffer(device.logical, buffer, nullptr);
        if (memory) {
            vkFreeMemory(device.logical, memory, nullptr);
        }
    }

} // namespace VKHelper
