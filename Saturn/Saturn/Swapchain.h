#ifndef	SWAPCHAIN_H
#define SWAPCHAIN_H

#include "VulkanCommon.h"
#include "PhysicalDevice.h"

class Swapchain
{
public:
	VkSwapchainKHR handle;
	VkSurfaceFormatKHR surfaceFormat;
	VkExtent2D extent;

	VkRenderPass renderPass;

	std::vector<VkImage> images;
	std::vector<VkImageView> views;

	/// TODO: necessary?
	//std::vector<VkFence> fences;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderDoneSemaphore;

	std::vector<VkCommandBuffer> commandBuffers;
	VkFence imageAvailable;
	uint32_t imageIndex = ~0U;

	std::vector<VkViewport>	viewports;
	std::vector<VkRect2D>	scissors;

	//VkImage			depthImage;
	//VkDeviceMemory	depthMemory;
	//VkImageView		depthView;
	std::vector<VkFramebuffer> framebuffers;

public:
	void Init(VkDevice _device, PhysicalDevice* _physicalDevice, VkSurfaceKHR _surface, uint32_t _targetImageCount, std::vector<VkCompositeAlphaFlagBitsKHR> _preferedCompositeAlphas, std::vector<VkPresentModeKHR> _preferedPresentModes, VkCommandPool _commandPool);

	void PrepareToRecord(VkDevice _device);
	VkCommandBuffer BeginRecording();
	void EndRecording();
};

#endif