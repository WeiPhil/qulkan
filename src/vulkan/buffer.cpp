#include "vulkan/vk_helper.hpp"

#include <string.h>
#include <optional>

namespace VKHelper {

    static VkResult createBuffer(VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkBuffer& buffer) {

        VK_CHECK_NOT_NULL(device, "device is VK_NULL_HANDLE");

        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VK_CHECK_RET(vkCreateBuffer(device, &bufferInfo, nullptr, &buffer));
        return VK_SUCCESS;
    }

    Buffer::Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage)
        : logicalDevice(physicalDevice, device), bufferSize(size), usage(usage) {
        VK_CHECK_FAIL(createBuffer(device, size, usage, buffer), "buffer creation failed");
    }

    Buffer::Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
        : logicalDevice(physicalDevice, device), bufferSize(size), usage(usage) {
        VK_CHECK_FAIL(createBuffer(device, size, usage, buffer), "buffer creation failed");
        VK_CHECK_FAIL(bind(properties), "buffer bind failed");
    }

    VkResult Buffer::bind(VkMemoryPropertyFlags properties) {

        VK_CHECK_NOT_NULL(logicalDevice.phyDev, "physicalDevice is VK_NULL_HANDLE");
        VK_CHECK_NULL(memory, "buffer is already binded to memory");

        this->memProperties = properties;
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(logicalDevice.dev, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        std::optional memType = findMemoryType(logicalDevice.phyDev, memRequirements.memoryTypeBits, properties);
        if (memType) {
            allocInfo.memoryTypeIndex = *memType;
        } else {
            return VK_ERROR_FEATURE_NOT_PRESENT;
        }

        VK_CHECK_RET(vkAllocateMemory(logicalDevice.dev, &allocInfo, nullptr, &memory));
        VK_CHECK_RET(vkBindBufferMemory(logicalDevice.dev, buffer, memory, 0));

        return VK_SUCCESS;
    }

    VkResult Buffer::mapAndCopy(void *dataToCopy, size_t size) {

        VK_CHECK_NOT_NULL(memory, "memory is VK_NULL_HANDLE");
        ASSERT_MSG(memProperties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, "memory is not mappable");

        void *data;
        VK_CHECK_RET(vkMapMemory(logicalDevice.dev, memory, 0, bufferSize, 0, &data));
        memcpy(data, dataToCopy, size);
        vkUnmapMemory(logicalDevice.dev, memory);

        return VK_SUCCESS;
    }

    Buffer::~Buffer() {
        vkDestroyBuffer(logicalDevice.dev, buffer, nullptr);
        if (memory) {
            vkFreeMemory(logicalDevice.dev, memory, nullptr);
        }
    }

} // namespace VKHelper
