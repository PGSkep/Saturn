//#pragma once

#ifndef	RENDERER_H
#define RENDERER_H

#include "../../Common/Utility/Math3D.h"

#include "VulkanCommon.h"
#include "Loader.h"

#include "Surface.h"
#include "PhysicalDevice.h"
#include "TransferManager.h"
#include "MemoryMap.h"
#include "Swapchain.h"

#include "Camera.h"

#include <vector>

class Renderer
{
private:
	struct Instance
	{
		VkInstance					instance = VK_NULL_HANDLE;
		VkDebugReportCallbackEXT	debugReportCallback = VK_NULL_HANDLE;
		std::vector<Surface>		surfaces;
		std::vector<PhysicalDevice>	physicalDevices;
	} instance;
	uint32_t graphicsGroupID = 0;
	uint32_t graphicsQueueID = 0;
	struct Device
	{
		VkDevice				device = VK_NULL_HANDLE;
		PhysicalDevice*			physicalDevice = nullptr;
		struct QueueGroup
		{
			uint32_t						familyIndex = -1;
			VkQueueFlags					flags = VK_NULL_HANDLE;
			PhysicalDevice::Presentability	presentability;

			std::vector<float>				priority;
			std::vector<VkQueue>			queues;

			VkCommandPool					commandPool;

			struct Requirements
			{
				VkQueueFlags	flags;
				bool			presentability;
				float			priority;
				uint32_t		count;

				static inline Requirements GetRequirements(VkQueueFlags	_flags, bool _presentability, float _priority, uint32_t _count)
				{
					return { _flags, _presentability, _priority, _count };
				}
			};
		};
		std::vector<QueueGroup>	queueGroups;

		VkCommandPool graphicsCommandPool;
		VkQueue graphicsQueue;
	} device;
	struct Presentation
	{
		VkFence renderDone;
		VkCommandBuffer submitCommandBuffer;
		VkQueue renderQueue;

		std::map<std::string, Swapchain> swapchainMap;
	} presentation;
	struct Compute {
		VkCommandBuffer commandBuffer;
	} compute;
	struct Memory
	{
		Structure instanceStruct;
		Structure speedStruct;
		Structure viewProjectionStruct;
		Structure deltaTimeStruct;

		TransferManager transferManager;

		struct Buffers
		{
			VkBuffer			handle;
			VkDeviceMemory		memory;
			VkBufferUsageFlags	usage;
			MemoryMap			memoryMap;

			void Init(VkDevice _device, PhysicalDevice* _physicalDevice, VkDeviceSize _size, VkMemoryPropertyFlags _memoryPropertyFlags, VkBufferUsageFlags _usage);
			MemoryMap::PoolNode* AllocatePool(const char* _name, VkDeviceSize _size, size_t _alignment, void* _userdata);
			//MemoryMap::BufferNode* AllocateBuffer(const char* _name, MemoryMap::PoolNode* _pool, VkDeviceSize _size, void* _userdata);
			void ShutDown();
		};
		struct Images
		{
			VkDeviceMemory		textureMemory;
			VkFormat			format;
			VkImageUsageFlags	usage;
			uint32_t			memoryTypeIndex;
			VkDeviceSize		alignment;
			MemoryMap			memoryMap;

			VkMemoryRequirements InitAlignment(VkDevice _device, VkFormat _format);
			void InitMemory(VkDevice _device, PhysicalDevice* _physicalDevice, VkDeviceSize _size, VkMemoryPropertyFlags _memoryPropertyFlags, VkMemoryRequirements _memoryRequirements);
			void Init(VkDevice _device, PhysicalDevice * _physicalDevice, VkDeviceSize _size, VkMemoryPropertyFlags _memoryPropertyFlags, VkFormat _format, VkImageUsageFlags _usage);
			MemoryMap::PoolNode* AllocatePool(const char* _name, VkDeviceSize _size, size_t _alignment, void* _userdata);
			MemoryMap::BufferNode* AllocateBuffer(const char * _name, MemoryMap::PoolNode * _pool, VkDeviceSize _size, void * _userdata, VkDevice _device);
			void ShutDown();
		};

		Buffers vertexBuffer;
		struct InstanceUserdata
		{
			uint32_t instanceCount;

			static inline InstanceUserdata Get(uint32_t _instanceCount)
			{
				return { _instanceCount };
			}
		};
		Buffers computeVertexBuffer;
		Buffers computeBuffer;
		struct IndexUserdata
		{
			VkIndexType	indexType;
			uint32_t	indexCount;

			static inline IndexUserdata Get(VkIndexType _indexType, uint32_t _indexCount)
			{
				return { _indexType, _indexCount };
			}
		};
		Buffers indexBuffer;
		Buffers uniformBuffer;

		struct ImageUserdata
		{
			VkImage			image;
			VkImageView		view;
			VkDeviceSize	validDataSize;
			//VkFormat		format;

			static inline ImageUserdata Get(VkImage _image, VkImageView _view, VkDeviceSize _validDataSize)
			{
				return { _image, _view, _validDataSize };
			}
		};
		Images RGBA8Images;
		Images depthImages;

	} memory;
	struct Sampler
	{
		std::map<std::string, VkSampler> samplers;
	} sampler;
	struct Descriptor
	{
		std::map<std::string, VkDescriptorSetLayout>	descriptorSetLayouts;
		std::map<std::string, VkDescriptorPool>			descriptorPools;
		std::map<std::string, VkDescriptorSet>			descriptorSets;

		struct DescriptorInfo
		{
			VkDescriptorSet descriptorSet;
			uint32_t binding;

			struct Uniform
			{
				//Memory::Buffers* uniformBufferPool;
				std::string uniformName;
			};
		};

	} descriptor;
	struct Shader
	{
		std::map<std::string, VkShaderModule> shaderModules;
	} shader;
	struct Pipeline
	{
		/// Layout
		std::map<std::string, VkPipelineLayout>	pipelineLayouts;

		/// Pipeline info
		// Shader Stage
		std::map<std::string, std::vector<VkPipelineShaderStageCreateInfo>>	shaderStages;
		// Input State
		std::map<std::string, std::vector<VkVertexInputBindingDescription>>		inputBindings;
		std::map<std::string, std::vector<VkVertexInputAttributeDescription>>	inputAttributes;
		std::map<std::string, VkPipelineVertexInputStateCreateInfo>				vertexInputStates;
		// Input Assembly
		std::map<std::string, VkPipelineInputAssemblyStateCreateInfo>			inputAssemblyStates;
		// Tessalation
		std::map<std::string, VkPipelineTessellationStateCreateInfo>			tessalationStates;
		// Viewport
		std::map<std::string, std::vector<VkViewport>>							viewports;
		std::map<std::string, std::vector<VkRect2D>>							scissors;
		std::map<std::string, VkPipelineViewportStateCreateInfo>				viewportStates;
		// Rasterization
		std::map<std::string, VkPipelineRasterizationStateCreateInfo>			rasterizationStates;
		// Multisample
		std::map<std::string, VkPipelineMultisampleStateCreateInfo>				multisampleStates;
		// Depth Stencil
		std::map<std::string, VkPipelineDepthStencilStateCreateInfo>			depthStencilStates;
		// Color Blend
		std::map<std::string, std::vector<VkPipelineColorBlendAttachmentState>>	colorBlendAttachment;
		std::map<std::string, VkPipelineColorBlendStateCreateInfo>				colorBlendStates;
		// Dynamic
		std::map<std::string, std::vector<VkDynamicState>>						dynamic;
		std::map<std::string, VkPipelineDynamicStateCreateInfo>					dynamicStates;

		/// Graphics Pipeline
		std::vector<VkGraphicsPipelineCreateInfo>	graphicsPipelineInfos;
		std::vector<VkPipeline>						graphicsPipelines;

		std::map<std::string, size_t>				graphicsPipelineIndices;

		/// Compute Pipeline
		std::vector<VkComputePipelineCreateInfo>	computePipelineInfos;
		std::vector<VkPipeline>						computePipelines;

		std::map<std::string, size_t>				computePipelineIndices;

	} pipeline;

public:
	void InitInstance(VkApplicationInfo _appInfo, std::vector<const char*> _instanceLayers, std::vector<const char*> _instanceExtensions, VkDebugReportFlagsEXT _debugFlags, PFN_vkDebugReportCallbackEXT _debugCallback);
	void AddSurface(const char* _name, const char* _title, int _width, int _height, WNDPROC _wndProc);
	void InitPhysicalDevice();

	struct QueueFamilyRequirements
	{
		VkQueueFlags		flags;
		bool				presentability;
		std::vector<float>	priorities;

		static QueueFamilyRequirements Get(VkQueueFlags _flags, bool _presentability, std::vector<float> _priorities)
		{
			QueueFamilyRequirements result;
			result.flags = _flags;
			result.presentability = _presentability;
			result.priorities = _priorities;
			return result;
		}
	};
	void InitDevice(size_t _physicalDeviceIndex, std::vector<const char*> _deviceExtensions, VkPhysicalDeviceFeatures _features, std::vector<QueueFamilyRequirements> _queueFamilyRequirements);

	void InitPresentation();

	void InitCompute();

	void InitMemory();

	void InitFramebuffers();

	void InitSamplers();

	void InitDescriptos();

	void InitShaders();
	void InitPipeline();

	void WriteComputeCommandBuffers();
	void UpdateBuffers(float _deltaTime, float _speedMultiplier, Math3D::Mat4 _viewProjection[2]);
	void WriteGraphicsCommandBuffers();
	void Present();

	void ShutDown();
};

#endif