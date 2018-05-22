#include "Swapchain.h"

void Swapchain::Init(VkDevice _device, PhysicalDevice* _physicalDevice, VkSurfaceKHR _surface, uint32_t _targetImageCount, std::vector<VkCompositeAlphaFlagBitsKHR> _preferedCompositeAlphas, std::vector<VkPresentModeKHR> _preferedPresentModes, VkCommandPool _commandPool)
{
	VkSurfaceCapabilitiesKHR* surfaceCapabilities = &_physicalDevice->surfaceProperties[0].surfaceCapabilities;
	std::vector<VkSurfaceFormatKHR>* surfaceFormats = &_physicalDevice->surfaceProperties[0].surfaceFormats;
	std::vector<VkPresentModeKHR>* presentModes = &_physicalDevice->surfaceProperties[0].presentModes;

	// Surface format
	if ((*surfaceFormats).size() == 1 && (*surfaceFormats)[0].format == VK_FORMAT_UNDEFINED)
		surfaceFormat = { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	else
		surfaceFormat = (*surfaceFormats)[0];

	// Extent
	extent = surfaceCapabilities->currentExtent;

	// Create swapchain
	{
		VkSwapchainCreateInfoKHR swapchainCreateInfo;
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.pNext = nullptr;
		swapchainCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		swapchainCreateInfo.surface = _surface;
		swapchainCreateInfo.minImageCount = _targetImageCount;
		swapchainCreateInfo.imageFormat = surfaceFormat.format;
		swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
		swapchainCreateInfo.imageExtent = extent;
		swapchainCreateInfo.imageArrayLayers = 1;
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainCreateInfo.queueFamilyIndexCount = 0;
		swapchainCreateInfo.pQueueFamilyIndices = nullptr;
		swapchainCreateInfo.preTransform = surfaceCapabilities->currentTransform;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
		swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

		// image count
		if (swapchainCreateInfo.minImageCount > surfaceCapabilities->maxImageCount)
			swapchainCreateInfo.minImageCount = surfaceCapabilities->maxImageCount;
		else if (swapchainCreateInfo.minImageCount < surfaceCapabilities->minImageCount)
			swapchainCreateInfo.minImageCount = surfaceCapabilities->minImageCount;

		// width
		if (swapchainCreateInfo.imageExtent.width > surfaceCapabilities->maxImageExtent.width)
			swapchainCreateInfo.imageExtent.width = surfaceCapabilities->maxImageExtent.width;
		else if (swapchainCreateInfo.imageExtent.width < surfaceCapabilities->minImageExtent.width)
			swapchainCreateInfo.imageExtent.width = surfaceCapabilities->minImageExtent.width;

		// height
		if (swapchainCreateInfo.imageExtent.height > surfaceCapabilities->maxImageExtent.height)
			swapchainCreateInfo.imageExtent.height = surfaceCapabilities->maxImageExtent.height;
		else if (swapchainCreateInfo.imageExtent.height < surfaceCapabilities->minImageExtent.height)
			swapchainCreateInfo.imageExtent.height = surfaceCapabilities->minImageExtent.height;

		// composite alpha
		for (size_t i = 0; i != _preferedCompositeAlphas.size(); ++i)
		{
			if ((_preferedCompositeAlphas[i] & surfaceCapabilities->supportedCompositeAlpha) == _preferedCompositeAlphas[i])
			{
				swapchainCreateInfo.compositeAlpha = _preferedCompositeAlphas[i];
				break;
			}
		}

		// present mode
		for (size_t iPrefered = 0; iPrefered != _preferedPresentModes.size(); ++iPrefered)
		{
			for (size_t iMode = 0; iMode != (*presentModes).size(); ++iMode)
			{
				if ((*presentModes)[iMode] == _preferedPresentModes[iPrefered])
				{
					swapchainCreateInfo.presentMode = _preferedPresentModes[iPrefered];
					iPrefered = _preferedPresentModes.size() - 1;
					break;
				}
			}
		}

		VulkanCommon::CreateSwapchain(_device, &swapchainCreateInfo, &handle);
	}

	// Create RenderPass
	{
		VkAttachmentDescription attachmentDescriptions[2];
		VkAttachmentDescription* colorAttachmentDescription = &attachmentDescriptions[0];
		colorAttachmentDescription->flags = VK_FLAGS_NONE;
		colorAttachmentDescription->format = surfaceFormat.format;
		colorAttachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDescription->finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		VkAttachmentDescription* depthAttachmentDescription = &attachmentDescriptions[1];
		depthAttachmentDescription->flags = VK_FLAGS_NONE;
		depthAttachmentDescription->format = _physicalDevice->depthFormat;
		depthAttachmentDescription->samples = VK_SAMPLE_COUNT_1_BIT;
		depthAttachmentDescription->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachmentDescription->storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription->stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachmentDescription->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachmentDescription->initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachmentDescription->finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentReference;
		colorAttachmentReference.attachment = 0;
		colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkAttachmentReference depthAttachmentReference;
		depthAttachmentReference.attachment = 1;
		depthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpassDescription;
		subpassDescription.flags = VK_FLAGS_NONE;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentReference;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = &depthAttachmentReference;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency subpassDependency;
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcAccessMask = 0;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.dependencyFlags = VK_FLAGS_NONE;

		VkRenderPassCreateInfo renderPassCreateInfo;
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.pNext = nullptr;
		renderPassCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		renderPassCreateInfo.attachmentCount = 2;
		renderPassCreateInfo.pAttachments = attachmentDescriptions;
		renderPassCreateInfo.subpassCount = 1;
		renderPassCreateInfo.pSubpasses = &subpassDescription;
		renderPassCreateInfo.dependencyCount = 1;
		renderPassCreateInfo.pDependencies = &subpassDependency;
		VulkanCommon::CreateRenderPass(_device, &renderPassCreateInfo, &renderPass);
	}

	// Images
	VulkanCommon::GetSwapchainImagesKHR(_device, handle, images);

	// Views
	{
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = surfaceFormat.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;

		views.resize(images.size());
		for (size_t iView = 0; iView != views.size(); ++iView)
		{
			imageViewCreateInfo.image = images[iView];
			VulkanCommon::CreateImageView(_device, &imageViewCreateInfo, &views[iView]);
		}
	}

	// Fences
	{
		/// TODO: necessary?
		//VkFenceCreateInfo fenceCreateInfo;
		//fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		//fenceCreateInfo.pNext = nullptr;
		//fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		//fences.resize(images.size());
		//for (size_t iFence = 0; iFence != views.size(); ++iFence)
		//	VulkanCommon::CreateFence(_device, &fenceCreateInfo, &fences[iFence]);

		// Image available
		VkFenceCreateInfo fenceCreateInfo;
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.pNext = nullptr;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VulkanCommon::CreateFence(_device, &fenceCreateInfo, &imageAvailable);
	}

	// Semaphores
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo;
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = nullptr;
		semaphoreCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		VulkanCommon::CreateSemaphore_(_device, &semaphoreCreateInfo, &imageAvailableSemaphore);
		VulkanCommon::CreateSemaphore_(_device, &semaphoreCreateInfo, &renderDoneSemaphore);
	}

	// Command buffers
	{
		commandBuffers.resize(images.size());
		VkCommandBufferAllocateInfo commandBufferAllocateInfo;
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.pNext = nullptr;
		commandBufferAllocateInfo.commandPool = _commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		commandBufferAllocateInfo.commandBufferCount = (uint32_t)commandBuffers.size();
		VulkanCommon::AllocateCommandBuffers(_device, &commandBufferAllocateInfo, commandBuffers.data());
	}

	// Viewports
	viewports = { VulkanCommon::GetVkViewport(0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f) };

	// Scissors
	scissors = { VulkanCommon::GetVkRect2D({ 0, 0 }, extent) };
}

void Swapchain::PrepareToRecord(VkDevice _device)
{
	VulkanCommon::WaitForFences(_device, 1, &imageAvailable, VK_TRUE, ~0U);
	VulkanCommon::ResetFences(_device, 1, &imageAvailable);
	VulkanCommon::AcquireNextImageKHR(_device, handle, ~0U, imageAvailableSemaphore, imageAvailable, &imageIndex);
}
VkCommandBuffer Swapchain::BeginRecording()
{
	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = renderPass;
	commandBufferInheritanceInfo.subpass = 0;
	commandBufferInheritanceInfo.framebuffer = framebuffers[imageIndex];
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	commandBufferInheritanceInfo.queryFlags = 0;
	commandBufferInheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
	VulkanCommon::BeginCommandBuffer(commandBuffers[imageIndex], &commandBufferBeginInfo);

	vkCmdSetViewport(commandBuffers[imageIndex], 0, (uint32_t)viewports.size(), viewports.data());
	vkCmdSetScissor(commandBuffers[imageIndex], 0, (uint32_t)scissors.size(), scissors.data());

	return commandBuffers[imageIndex];
}
void Swapchain::EndRecording()
{
	VulkanCommon::EndCommandBuffer(commandBuffers[imageIndex]);
}
