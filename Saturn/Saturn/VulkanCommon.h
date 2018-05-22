#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#define VK_RESERVED_FOR_FUTURE_USE 0
#define VK_IGNORED 0
#define VK_FLAGS_NONE 0

#include "../../Common/Utility/Error.h"
#include "../../Common/Utility/Console.h"

#include <vector>
#include "../../Common/Utility/Structure.h"

namespace VulkanCommon
{
	// Error
	std::string GetStrFromVkResult(VkResult _result);

	// Utility
	VkPhysicalDeviceFeatures PhysicalDevivceGetFeaturesFromStrArr(std::vector<const char*> _featureNames);
	uint32_t FindMemoryType(VkPhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties, uint32_t _memoryTypeBits, VkMemoryPropertyFlags _memoryPropertyFlags);
	uint32_t FindFittingMemoryType(VkPhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties, uint32_t _memoryTypeBits, VkMemoryPropertyFlags _memoryPropertyFlags, VkDeviceSize _size);

	// Utility2
	VkAccessFlags GetSrcAccessMask(VkImageLayout _oldLayout, VkImageLayout _newLayout);
	VkAccessFlags GetDstAccessMask(VkImageLayout _oldLayout, VkImageLayout _newLayout);
	VkPipelineStageFlags GetSrcStageMask(VkImageLayout _oldLayout, VkImageLayout _newLayout);
	VkPipelineStageFlags GetDstStageMask(VkImageLayout _oldLayout, VkImageLayout _newLayout);

	// Instance
	VkApplicationInfo GetVkApplicationInfo(const char* _pApplicationName, uint32_t _applicationVersion, const char* _pEngineName, uint32_t _engineVersion, uint32_t _apiVersion);
	void CreateInstance(VkInstanceCreateInfo* _pCreateInfo, VkInstance& _pInstance);

	// Debug
	VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT _flags, VkDebugReportObjectTypeEXT _objType, uint64_t _obj, size_t _location, int32_t _code, const char* _layerPrefix, const char* _msg, void* _userData);
	void CreateDebugReportCallbackEXT(VkInstance _instance, const VkDebugReportCallbackCreateInfoEXT* _pCreateInfo, VkDebugReportCallbackEXT& _pCallback);

	// Surface
	LRESULT WINAPI MainWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
	void CreateWin32SurfaceKHR(VkInstance _instance, const VkWin32SurfaceCreateInfoKHR* _pCreateInfo, VkSurfaceKHR& _surface);

	// Physical Device
	void EnumeratePhysicalDevices(VkInstance _instance, std::vector<VkPhysicalDevice>& _physicalDevices);
	void GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice _physicalDevice, uint32_t _queueFamilyIndex, VkSurfaceKHR _surface, VkBool32* _pSupported);
	void EnumerateDeviceLayerProperties(VkPhysicalDevice _physicalDevice, std::vector<VkLayerProperties>& _layers);
	void EnumerateDeviceExtensionProperties(VkPhysicalDevice _physicalDevice, std::vector<VkExtensionProperties>& _extensions);
	void GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, VkSurfaceCapabilitiesKHR* _pSurfaceCapabilities);
	void GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, std::vector<VkSurfaceFormatKHR>& _pSurfaceFormats);
	void GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, std::vector<VkPresentModeKHR>& _pPresentModes);

	// Device
	void CreateDevice(VkPhysicalDevice _physicalDevice, const VkDeviceCreateInfo* _pCreateInfo, VkDevice& _pDevice);

	// Command
	void CreateCommandPool(VkDevice _device, const VkCommandPoolCreateInfo* _pCreateInfo, VkCommandPool* _pCommandPool);
	void AllocateCommandBuffers(VkDevice _device, const VkCommandBufferAllocateInfo* _pAllocateInfo, VkCommandBuffer* _pCommandBuffers);
	void BeginCommandBuffer(VkCommandBuffer _commandBuffer, const VkCommandBufferBeginInfo* _pBeginInfo);
	void EndCommandBuffer(VkCommandBuffer _commandBuffer);
	void QueueSubmit(VkQueue _queue, uint32_t _submitCount, const VkSubmitInfo* _pSubmits, VkFence _fence);

	// Sync
	void CreateFence(VkDevice _device, const VkFenceCreateInfo* _pCreateInfo, VkFence* _pFence);
	void CreateSemaphore_(VkDevice _device, const VkSemaphoreCreateInfo* _pCreateInfo, VkSemaphore* _pSemaphore);
	void WaitForFences(VkDevice _device, uint32_t _fenceCount, const VkFence* _pFences, VkBool32 _waitAll, uint64_t _timeout);
	void ResetFences(VkDevice _device, uint32_t _fenceCount, const VkFence* _pFences);

	// Memory
	VkImageCreateInfo GetVkImageCreateInfo(uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage);
	void CreateBuffer(VkDevice _device, const VkBufferCreateInfo* _pCreateInfo, VkBuffer* _pBuffer);
	void CreateImage(VkDevice _device, const VkImageCreateInfo* _pCreateInfo, VkImage* _pImage);
	void CreateImageView(VkDevice _device, const VkImageViewCreateInfo* _pCreateInfo, VkImageView* _pView);
	void AllocateMemory(VkDevice _device, const VkMemoryAllocateInfo* _pAllocateInfo, VkDeviceMemory* _pMemory);
	void BindBufferMemory(VkDevice _device, VkBuffer _buffer, VkDeviceMemory _memory, VkDeviceSize _memoryOffset);
	void BindImageMemory(VkDevice _device, VkImage _image, VkDeviceMemory _memory, VkDeviceSize _memoryOffset);
	void MapMemory(VkDevice _device, VkDeviceMemory _memory, VkDeviceSize _offset, VkDeviceSize _size, void** _ppData);

	// Presentation
	VkViewport GetVkViewport(float _x, float _y, float _width, float _height, float _minDepth, float _maxDepth);
	VkRect2D GetVkRect2D(VkOffset2D _offset, VkExtent2D _extent);
	void CreateSwapchain(VkDevice _device, const VkSwapchainCreateInfoKHR* _pCreateInfo, VkSwapchainKHR* _pSwapchain);
	void CreateRenderPass(VkDevice _device, const VkRenderPassCreateInfo* _pCreateInfo, VkRenderPass* _pRenderPass);
	void GetSwapchainImagesKHR(VkDevice _device, VkSwapchainKHR _swapchain, std::vector<VkImage>& _images);
	void CreateFramebuffer(VkDevice _device, const VkFramebufferCreateInfo* _pCreateInfo, VkFramebuffer* _pFramebuffer);
	void AcquireNextImageKHR(VkDevice _device, VkSwapchainKHR _swapchain, uint64_t _timeout, VkSemaphore _semaphore, VkFence _fence, uint32_t* _pImageIndex);
	void QueuePresentKHR(VkQueue _queue, const VkPresentInfoKHR* _pPresentInfo);

	// Sampler
	void CreateSampler(VkDevice _device, const VkSamplerCreateInfo* _pCreateInfo, VkSampler* _pSampler);

	// Descriptor
	VkDescriptorSetLayoutBinding GetVkDescriptorSetLayoutBinding(uint32_t _binding, VkDescriptorType _descriptorType, uint32_t _descriptorCount, VkShaderStageFlags _stageFlags, const VkSampler* _pImmutableSamplers);
	VkDescriptorPoolSize GetVkDescriptorPoolSize(VkDescriptorType _type, uint32_t _descriptorCount);
	void CreateDescriptorSetLayout(VkDevice _device, const VkDescriptorSetLayoutCreateInfo* _pCreateInfo, VkDescriptorSetLayout* _pSetLayout);
	void CreateDescriptorPool(VkDevice _device, const VkDescriptorPoolCreateInfo* _pCreateInfo, VkDescriptorPool* _pDescriptorPool);
	void AllocateDescriptorSets(VkDevice _device, const VkDescriptorSetAllocateInfo* _pAllocateInfo, VkDescriptorSet* _pDescriptorSets);

	// Shader
	void CreateShaderModule(VkDevice _device, const VkShaderModuleCreateInfo* _pCreateInfo, VkShaderModule* _pShaderModule);

	// Pipeline Layout
	VkPushConstantRange GetVkPushConstantRange(VkShaderStageFlags _stageFlags, uint32_t _offset, uint32_t _size);
	void CreatePipelineLayout(VkDevice _device, const VkPipelineLayoutCreateInfo* _pCreateInfo, VkPipelineLayout* _pPipelineLayout);

	// Pipeline
	struct BindingDescriptionInfo
	{
		Structure			structure;
		VkVertexInputRate	inputRate;

		static inline BindingDescriptionInfo Get(Structure _structure, VkVertexInputRate _inputRate)
		{
			BindingDescriptionInfo bindingDescriptionInfo;
			bindingDescriptionInfo.structure = _structure;
			bindingDescriptionInfo.inputRate = _inputRate;
			return bindingDescriptionInfo;
		}
	};
	std::vector<VkVertexInputBindingDescription> GetVertexInputBindingDescriptions(std::vector<BindingDescriptionInfo> _info);
	std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions(std::vector<Structure> _structures);
	VkPipelineColorBlendAttachmentState GetVkPipelineColorBlendAttachmentState(VkBool32 _blendEnable, VkBlendFactor _srcColorBlendFactor, VkBlendFactor _dstColorBlendFactor, VkBlendOp _colorBlendOp, VkBlendFactor _srcAlphaBlendFactor, VkBlendFactor _dstAlphaBlendFactor, VkBlendOp _alphaBlendOp, VkColorComponentFlags _colorWriteMask);

	VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo(VkShaderStageFlagBits _stage, VkShaderModule _module);
	VkPipelineVertexInputStateCreateInfo GetVkPipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>* _vertexBindings, const std::vector<VkVertexInputAttributeDescription>* _vertexAttributes);
	VkPipelineInputAssemblyStateCreateInfo GetVkPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology _topology, VkBool32 _primitiveRestartEnable);
	VkPipelineTessellationStateCreateInfo GetVkPipelineTessellationStateCreateInfo(uint32_t _patchControlPoints);
	VkPipelineViewportStateCreateInfo GetVkPipelineViewportStateCreateInfo(const std::vector<VkViewport> _viewports, const std::vector<VkRect2D> _scissors);
	VkPipelineRasterizationStateCreateInfo GetVkPipelineRasterizationStateCreateInfo(VkBool32 _depthClampEnable, VkBool32 _rasterizerDiscardEnable, VkPolygonMode _polygonMode, VkCullModeFlags _cullMode, VkFrontFace _frontFace, VkBool32 _depthBiasEnable, float _depthBiasConstantFactor, float _depthBiasClamp, float _depthBiasSlopeFactor, float _lineWidth);
	VkPipelineMultisampleStateCreateInfo GetVkPipelineMultisampleStateCreateInfo(VkSampleCountFlagBits _rasterizationSamples, VkBool32 _sampleShadingEnable, float _minSampleShading, const VkSampleMask* _pSampleMask, VkBool32 _alphaToCoverageEnable, VkBool32 _alphaToOneEnable);
	VkPipelineDepthStencilStateCreateInfo GetVkPipelineDepthStencilStateCreateInfo(VkBool32 _depthTestEnable, VkBool32 _depthWriteEnable, VkCompareOp _depthCompareOp, VkBool32 _depthBoundsTestEnable, VkBool32 _stencilTestEnable, VkStencilOpState _front, VkStencilOpState _back, float _minDepthBounds, float _maxDepthBounds);
	VkPipelineColorBlendStateCreateInfo GetVkPipelineColorBlendStateCreateInfo(VkBool32 _logicOpEnable, VkLogicOp _logicOp, const std::vector<VkPipelineColorBlendAttachmentState>* _colorBlendAttachments, float _blendConstants0, float _blendConstants1, float _blendConstants2, float _blendConstants3);
	VkPipelineDynamicStateCreateInfo GetVkPipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>* _dynamicStates);
	VkGraphicsPipelineCreateInfo GetVkGraphicsPipelineCreateInfo(uint32_t _stageCount, const VkPipelineShaderStageCreateInfo* _pStages, const VkPipelineVertexInputStateCreateInfo* _pVertexInputState, const VkPipelineInputAssemblyStateCreateInfo* _pInputAssemblyState, const VkPipelineTessellationStateCreateInfo* _pTessellationState, const VkPipelineViewportStateCreateInfo* _pViewportState, const VkPipelineRasterizationStateCreateInfo* _pRasterizationState, const VkPipelineMultisampleStateCreateInfo* _pMultisampleState, const VkPipelineDepthStencilStateCreateInfo* _pDepthStencilState, const VkPipelineColorBlendStateCreateInfo* _pColorBlendState, const VkPipelineDynamicStateCreateInfo* _pDynamicState, VkPipelineLayout _layout, VkRenderPass _renderPass, VkPipeline _basePipelineHandle);
	void CreateGraphicsPipelines(VkDevice _device, uint32_t _createInfoCount, const VkGraphicsPipelineCreateInfo* _pCreateInfos, VkPipeline* _pPipelines);
	VkComputePipelineCreateInfo GetVkComputePipelineCreateInfo(VkPipelineShaderStageCreateInfo _pipelineShaderStageCreateInfo, VkPipelineLayout _layout, VkPipeline _basePipelineHandle);
	void CreateComputePipelines(VkDevice _device, uint32_t _createInfoCount, const VkComputePipelineCreateInfo* _pCreateInfos, VkPipeline* _pPipelines);

	// Cmd
	void CmdPipelineBarrier(VkCommandBuffer _commandBuffer, VkImage _image, VkImageLayout _oldLayout, VkImageLayout _newLayout);
}