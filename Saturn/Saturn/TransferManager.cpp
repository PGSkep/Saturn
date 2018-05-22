#include "TransferManager.h"

void TransferManager::Init(VkDevice _device, PhysicalDevice* _physicalDevice, VkCommandPool _commandPool, VkQueue _transferQueue, VkDeviceSize _transferBufferSize)
{
	device = _device;
	queue = _transferQueue;
	physicalDevice = _physicalDevice;

	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = _commandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;
	VulkanCommon::AllocateCommandBuffers(_device, &commandBufferAllocateInfo, &commandBuffer);

	VkFenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	VulkanCommon::CreateFence(_device, &fenceCreateInfo, &fence);

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.pNext = nullptr;
	semaphoreCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
	VulkanCommon::CreateSemaphore_(_device, &semaphoreCreateInfo, &semaphore);

	if(_transferBufferSize > 0)
		Resize(_transferBufferSize);
}
void TransferManager::Resize(VkDeviceSize _transferBufferSize)
{
	bufferSize = _transferBufferSize;

	if (buffer != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(device, buffer, nullptr);
		vkFreeMemory(device, memory, nullptr);
	}

	VkBufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = VK_FLAGS_NONE;
	bufferCreateInfo.size = bufferSize;
	bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;
	VulkanCommon::CreateBuffer(device, &bufferCreateInfo, &buffer);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(device, buffer, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = VulkanCommon::FindFittingMemoryType(physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _transferBufferSize);
	VulkanCommon::AllocateMemory(device, &memoryAllocateInfo, &memory);

	VulkanCommon::BindBufferMemory(device, buffer, memory, 0);
}
void TransferManager::ShutDown()
{
	vkDestroyBuffer(device, buffer, nullptr);
	vkFreeMemory(device, memory, nullptr);

	vkDestroyFence(device, fence, nullptr);
	vkDestroySemaphore(device, semaphore, nullptr);
}

VkCommandBuffer TransferManager::BeginCommandBuffer()
{
	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	VulkanCommon::BeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);

	return commandBuffer;
}
void TransferManager::EndCommandBuffer()
{
	VulkanCommon::EndCommandBuffer(commandBuffer);
}
void TransferManager::SubmitCommandBuffer()
{
	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = 0;
	submitInfo.pWaitSemaphores = nullptr;
	submitInfo.pWaitDstStageMask = nullptr;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;
	submitInfo.signalSemaphoreCount = 0;
	submitInfo.pSignalSemaphores = nullptr;
	VulkanCommon::QueueSubmit(queue, 1, &submitInfo, fence);
}

void TransferManager::WaitResetTransfer()
{
	VulkanCommon::WaitForFences(device, 1, &fence, VK_TRUE, ~0U);
	VulkanCommon::ResetFences(device, 1, &fence);
}
void TransferManager::WaitTransfer()
{
	VulkanCommon::WaitForFences(device, 1, &fence, VK_TRUE, ~0U);
}

void TransferManager::CpuTransfer(void * _data, size_t _dataSize, VkBuffer _target, VkDeviceSize _targetOffset)
{
	VulkanCommon::WaitForFences(device, 1, &fence, VK_TRUE, ~0U);
	VulkanCommon::ResetFences(device, 1, &fence);

	if (_dataSize > bufferSize)
		Resize(_dataSize);

	void* data = nullptr;
	VulkanCommon::MapMemory(device, memory, 0, _dataSize, &data);
	memcpy((void*)((size_t)data), _data, _dataSize);
	vkUnmapMemory(device, memory);

	BeginCommandBuffer();

	VkBufferCopy bufferCopy;
	bufferCopy.srcOffset = 0;
	bufferCopy.dstOffset = _targetOffset;
	bufferCopy.size = _dataSize;
	vkCmdCopyBuffer(commandBuffer, buffer, _target, 1, &bufferCopy);

	EndCommandBuffer();

	SubmitCommandBuffer();
}
void TransferManager::CpuTransfer(void * _data, size_t _dataSize, VkImage _target, VkDeviceSize _targetOffset, VkExtent3D _imageExtent)
{
	VulkanCommon::WaitForFences(device, 1, &fence, VK_TRUE, ~0U);
	VulkanCommon::ResetFences(device, 1, &fence);

	if (_dataSize > bufferSize)
		Resize(_dataSize);

	void* data = nullptr;
	VulkanCommon::MapMemory(device, memory, 0, _dataSize, &data);
	memcpy((void*)((size_t)data), _data, _dataSize);
	vkUnmapMemory(device, memory);

	BeginCommandBuffer();

	VulkanCommon::CmdPipelineBarrier(commandBuffer, _target, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

	VkBufferImageCopy bufferImageCopy;
	bufferImageCopy.bufferOffset = 0;
	bufferImageCopy.bufferRowLength = 0;
	bufferImageCopy.bufferImageHeight = 0;
	bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	bufferImageCopy.imageSubresource.mipLevel = 0;
	bufferImageCopy.imageSubresource.baseArrayLayer = 0;
	bufferImageCopy.imageSubresource.layerCount = 1;
	bufferImageCopy.imageOffset = { 0, 0, 0 };
	bufferImageCopy.imageExtent = _imageExtent;
	vkCmdCopyBufferToImage(commandBuffer, buffer, _target, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &bufferImageCopy);

	VulkanCommon::CmdPipelineBarrier(commandBuffer, _target, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	EndCommandBuffer();

	SubmitCommandBuffer();
}
void TransferManager::GpuTransfer(VkBuffer _srcBuffer, VkDeviceSize _srcOffset, VkBuffer _dstBuffer, VkDeviceSize _dstOffset, VkDeviceSize _size)
{
	VulkanCommon::WaitForFences(device, 1, &fence, VK_TRUE, ~0U);
	VulkanCommon::ResetFences(device, 1, &fence);

	if (_size > bufferSize)
		Resize(_size);

	BeginCommandBuffer();

	VkBufferCopy bufferCopy;
	bufferCopy.srcOffset = _srcOffset;
	bufferCopy.dstOffset = _dstOffset;
	bufferCopy.size = _size;
	vkCmdCopyBuffer(commandBuffer, _srcBuffer, _dstBuffer, 1, &bufferCopy);

	EndCommandBuffer();

	SubmitCommandBuffer();
}
void TransferManager::FillTransfer(VkDeviceSize _dstOffset, VkDeviceSize _size, void * _srcData)
{
	if (_size > bufferSize)
		Resize(_size);

	void* data = nullptr;
	VulkanCommon::MapMemory(device, memory, _dstOffset, _size, &data);
	memcpy((void*)((size_t)data), _srcData, _size);
	vkUnmapMemory(device, memory);
}
