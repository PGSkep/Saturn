#ifndef	TRANSFER_MANAGER_H
#define TRANSFER_MANAGER_H

#include "VulkanCommon.h"
#include "PhysicalDevice.h"

class TransferManager
{
	VkDevice		device;
	PhysicalDevice* physicalDevice;
	VkQueue			queue;
	VkCommandBuffer	commandBuffer;
	VkFence			fence;
	VkSemaphore		semaphore;

	VkBuffer		buffer = VK_NULL_HANDLE;
	VkDeviceMemory	memory = VK_NULL_HANDLE;

	VkDeviceSize	bufferSize = 0;

public:
	void Init(VkDevice _device, PhysicalDevice* _physicalDevice, VkCommandPool _commandPool, VkQueue _transferQueue, VkDeviceSize _transferBufferSize);
	void Resize(VkDeviceSize _transferBufferSize);
	void ShutDown();

	VkCommandBuffer BeginCommandBuffer();
	void EndCommandBuffer();
	void SubmitCommandBuffer();

	void WaitResetTransfer();
	void WaitTransfer();

	void CpuTransfer(void* _data, size_t _dataSize, VkBuffer _target, VkDeviceSize _targetOffset);
	void CpuTransfer(void * _data, size_t _dataSize, VkImage _target, VkDeviceSize _targetOffset, VkExtent3D _imageExtent);
	void GpuTransfer(VkBuffer _srcBuffer, VkDeviceSize _srcOffset, VkBuffer _dstBuffer, VkDeviceSize _dstOffset, VkDeviceSize _size);
	void FillTransfer(VkDeviceSize _dstOffset, VkDeviceSize _size, void* _srcData);
};

#endif