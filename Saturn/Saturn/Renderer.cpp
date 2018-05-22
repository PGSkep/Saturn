#include "Renderer.h"

#include <random>

// Buffers
void Renderer::Memory::Buffers::Init(VkDevice _device, PhysicalDevice* _physicalDevice, VkDeviceSize _size, VkMemoryPropertyFlags _memoryPropertyFlags, VkBufferUsageFlags _usage)
{
	usage = _usage;
	memoryMap.InitHead(_size, true);

	VkBufferCreateInfo bufferCreateInfo;
	bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferCreateInfo.pNext = nullptr;
	bufferCreateInfo.flags = VK_FLAGS_NONE;
	bufferCreateInfo.size = _size;
	bufferCreateInfo.usage = _usage;
	bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	bufferCreateInfo.queueFamilyIndexCount = 0;
	bufferCreateInfo.pQueueFamilyIndices = nullptr;
	VulkanCommon::CreateBuffer(_device, &bufferCreateInfo, &handle);

	VkMemoryRequirements memoryRequirements;
	vkGetBufferMemoryRequirements(_device, handle, &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocateInfo;
	memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocateInfo.pNext = nullptr;
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = VulkanCommon::FindFittingMemoryType(_physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, _memoryPropertyFlags, _size);
	VulkanCommon::AllocateMemory(_device, &memoryAllocateInfo, &memory);

	VulkanCommon::BindBufferMemory(_device, handle, memory, 0);
}
MemoryMap::PoolNode* Renderer::Memory::Buffers::AllocatePool(const char * _name, VkDeviceSize _size, size_t _alignment, void* _userdata)
{
	MemoryMap::PoolNode* pool = memoryMap.AllocatePool(_name, _size, _alignment, _userdata, true);
	return pool;
}
//MemoryMap::BufferNode* Renderer::Memory::Buffers::AllocateBuffer(const char * _name, MemoryMap::PoolNode* _pool, VkDeviceSize _size, void* _userdata)
//{
//	MemoryMap::BufferNode* buffer = _pool->AllocateBuffer(_name, _size, _userdata);
//	return buffer;
//}
void Renderer::Memory::Buffers::ShutDown()
{
	memoryMap.ShutDown();
}

// Images
VkMemoryRequirements Renderer::Memory::Images::InitAlignment(VkDevice _device, VkFormat _format)
{
	// Set format
	format = _format;

	// Create dummy image to get memoryTypeBits
	VkImage dummyImage;
	VulkanCommon::CreateImage(_device, &VulkanCommon::GetVkImageCreateInfo(800, 600, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT), &dummyImage);

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(_device, dummyImage, &memoryRequirements);

	// Set alignment
	alignment = memoryRequirements.alignment;

	vkDestroyImage(_device, dummyImage, nullptr);

	return memoryRequirements;
}
void Renderer::Memory::Images::InitMemory(VkDevice _device, PhysicalDevice* _physicalDevice, VkDeviceSize _size, VkMemoryPropertyFlags _memoryPropertyFlags, VkMemoryRequirements _memoryRequirements)
{
	// Align size to fit image alignment
	size_t alignmentPadding = _size % alignment;
	size_t alignedSize = _size + alignmentPadding;

	// Get memoryTypeIndex
	memoryTypeIndex = VulkanCommon::FindFittingMemoryType(_physicalDevice->memoryProperties, _memoryRequirements.memoryTypeBits, _memoryPropertyFlags, alignedSize);

	// Allocate memory
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = alignedSize;
	allocInfo.memoryTypeIndex = memoryTypeIndex;

	VulkanCommon::AllocateMemory(_device, &allocInfo, &textureMemory);

	// Setup head
	memoryMap.InitHead(alignedSize, true);
}
void Renderer::Memory::Images::Init(VkDevice _device, PhysicalDevice * _physicalDevice, VkDeviceSize _size, VkMemoryPropertyFlags _memoryPropertyFlags, VkFormat _format, VkImageUsageFlags _usage)
{
	usage = _usage;
	format = _format;

	VkImage dummyImage;
	VulkanCommon::CreateImage(_device,
		&VulkanCommon::GetVkImageCreateInfo(1, 1, format, VK_IMAGE_TILING_OPTIMAL, usage),
		&dummyImage);

	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(_device, dummyImage, &memoryRequirements);
	vkDestroyImage(_device, dummyImage, nullptr);

	alignment =  memoryRequirements.alignment;

	// Get memoryTypeIndex
	memoryTypeIndex = VulkanCommon::FindFittingMemoryType(_physicalDevice->memoryProperties, memoryRequirements.memoryTypeBits, _memoryPropertyFlags, _size);

	// Allocate memory
	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = _size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;
	VulkanCommon::AllocateMemory(_device, &allocInfo, &textureMemory);

	// Setup head
	memoryMap.InitHead(_size, true);
}
MemoryMap::PoolNode* Renderer::Memory::Images::AllocatePool(const char* _name, VkDeviceSize _size, size_t _alignment, void* _userdata)
{
	MemoryMap::PoolNode* pool = memoryMap.AllocatePool(_name, _size, _alignment, _userdata, true);
	return pool;
}
MemoryMap::BufferNode* Renderer::Memory::Images::AllocateBuffer(const char* _name, MemoryMap::PoolNode* _pool, VkDeviceSize _size, void* _userdata, VkDevice _device)
{
	// Align size to fit image alignment
	size_t alignmentPadding = _size % alignment;
	size_t alignedSize = _size + alignmentPadding;

	MemoryMap::BufferNode* buffer = _pool->AllocateBuffer(_name, alignedSize, _userdata);

	return buffer;
}
void Renderer::Memory::Images::ShutDown()
{
	memoryMap.ShutDown();
}

// Renderer
void Renderer::InitInstance(VkApplicationInfo _appInfo, std::vector<const char*> _instanceLayers, std::vector<const char*> _instanceExtensions, VkDebugReportFlagsEXT _debugFlags, PFN_vkDebugReportCallbackEXT _debugCallback)
{
	/// INSTANCE
	VkValidationCheckEXT disabled_check = VK_VALIDATION_CHECK_ALL_EXT;
	VkValidationFlagsEXT validationFlags;
	validationFlags.sType = VK_STRUCTURE_TYPE_VALIDATION_FLAGS_EXT;
	validationFlags.pNext = nullptr;
	validationFlags.disabledValidationCheckCount = 1;
	validationFlags.pDisabledValidationChecks = &disabled_check;
	VkDebugReportCallbackCreateInfoEXT debugReportCallbackCreateInfo;
	debugReportCallbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
	debugReportCallbackCreateInfo.pNext = (void*)&validationFlags;
	debugReportCallbackCreateInfo.flags = _debugFlags;
	debugReportCallbackCreateInfo.pfnCallback = _debugCallback;
	debugReportCallbackCreateInfo.pUserData = nullptr;
	VkInstanceCreateInfo instanceCreateInfo;
	instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	if (_debugFlags == 0 || _debugCallback == nullptr)
		instanceCreateInfo.pNext = nullptr;
	else
		instanceCreateInfo.pNext = &debugReportCallbackCreateInfo;
	instanceCreateInfo.flags = VK_FLAGS_NONE;
	instanceCreateInfo.pApplicationInfo = &_appInfo;
	instanceCreateInfo.enabledLayerCount = (uint32_t)_instanceLayers.size();
	instanceCreateInfo.ppEnabledLayerNames = _instanceLayers.data();
	instanceCreateInfo.enabledExtensionCount = (uint32_t)_instanceExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = _instanceExtensions.data();
	VulkanCommon::CreateInstance(&instanceCreateInfo, instance.instance);

	/// DEBUG
	debugReportCallbackCreateInfo.pNext = nullptr;
	if (!(_debugFlags == 0 || _debugCallback == nullptr))
		VulkanCommon::CreateDebugReportCallbackEXT(instance.instance, &debugReportCallbackCreateInfo, instance.debugReportCallback);

	/// SURFACE
	Surface::SetHInstance();
}
void Renderer::AddSurface(const char * _name, const char * _title, int _width, int _height, WNDPROC _wndProc)
{
	/// SURFACE
	instance.surfaces.push_back({});
	instance.surfaces[instance.surfaces.size() - 1].Init(_name, _title, _width, _height, _wndProc, instance.instance);
}
void Renderer::InitPhysicalDevice()
{
	/// PHYSICAL DEVICE
	std::vector<VkPhysicalDevice> vkPhysicalDevices;
	VulkanCommon::EnumeratePhysicalDevices(instance.instance, vkPhysicalDevices);

	std::vector<VkSurfaceKHR> vkSurfaces(instance.surfaces.size());
	for (size_t i = 0; i != instance.surfaces.size(); ++i)
		vkSurfaces[i] = instance.surfaces[i].GetHandle();

	instance.physicalDevices.resize(vkPhysicalDevices.size());
	for (size_t iPhysicalDevice = 0; iPhysicalDevice != instance.physicalDevices.size(); ++iPhysicalDevice)
		instance.physicalDevices[iPhysicalDevice].Init(vkPhysicalDevices[iPhysicalDevice], vkSurfaces, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT });
}

void Renderer::InitDevice(size_t _physicalDeviceIndex, std::vector<const char*> _deviceExtensions, VkPhysicalDeviceFeatures _features, std::vector<QueueFamilyRequirements> _queueFamilyRequirements)
{
	bool found;
	std::vector<std::vector<uint32_t>> validIndices;
	uint32_t validIndexPick = 0; // HARDCODED

	device.physicalDevice = &instance.physicalDevices[_physicalDeviceIndex];

	// validate extensions
	for (size_t i = 0; i != _deviceExtensions.size(); ++i)
	{
		found = false;
		std::vector<VkExtensionProperties>* availableExtensions = &device.physicalDevice->extensions;
		for (size_t j = 0; j != availableExtensions->size(); ++j)
		{
			if (strcmp(_deviceExtensions[i], (*availableExtensions)[j].extensionName) == 0)
				found = true;
		}
		if (found == false)
			Error::Report(_deviceExtensions[i]);
	}

	// validate features
	for (size_t i = 0; i != sizeof(VkPhysicalDeviceFeatures) / sizeof(VkBool32); ++i)
	{
		if (((VkBool32*)&_features)[i] == VK_TRUE && ((VkBool32*)&device.physicalDevice->features)[i] == VK_FALSE)
		{
			Error::Report(("feature " + std::to_string(i)).c_str());
		}
	}

	// Get valid indices
	size_t queueCount = device.physicalDevice->queueFamilyProperties.size();
	// get number of iterations
	size_t numberOfIterations = (size_t)pow(queueCount, _queueFamilyRequirements.size());

	// iterate
	std::vector<uint32_t> indices(_queueFamilyRequirements.size());
	for (size_t iterationNum = 0; iterationNum != numberOfIterations; ++iterationNum)
	{
		// overflow to next when needed
		if (_queueFamilyRequirements.size() > 1)
		{
			for (size_t iReq = 0; iReq != _queueFamilyRequirements.size(); ++iReq)
			{
				if (indices[iReq] == indices.size())
				{
					indices[iReq] = 0;
					++indices[iReq + 1];
				}
			}
		}

		// check if indice group is valid
		std::vector<uint32_t> familyQueueUseCount(queueCount);
		bool valid = true;
		for (size_t iReq = 0; iReq != _queueFamilyRequirements.size(); ++iReq)
		{
			uint32_t familyIndex = indices[iReq];

			// count
			if (device.physicalDevice->queueFamilyProperties[familyIndex].queueCount < familyQueueUseCount[familyIndex] + _queueFamilyRequirements[iReq].priorities.size())
			{
				valid = false;
				break;
			}

			// flags
			if ((_queueFamilyRequirements[iReq].flags & device.physicalDevice->queueFamilyProperties[familyIndex].queueFlags) != _queueFamilyRequirements[iReq].flags)
			{
				valid = false;
				break;
			}

			// presentation
			if (_queueFamilyRequirements[iReq].presentability == true)
			{
				if (device.physicalDevice->queueFamilyPresentability[familyIndex].win32Presentability == VK_FALSE)
				{
					valid = false;
					break;
				}

				VkBool32 surfaceSupport = VK_TRUE;
				device.physicalDevice->queueFamilyPresentability[familyIndex].perSurfacePresentability.resize(instance.surfaces.size());
				for (size_t iSurface = 0; iSurface != instance.surfaces.size(); ++iSurface)
				{
					if (device.physicalDevice->queueFamilyPresentability[familyIndex].perSurfacePresentability[iSurface] == VK_FALSE)
					{
						valid = false;
						break;
					}
				}
				if (!valid)
				{
					break;
				}
			}
		}

		if (valid)
		{
			for (size_t i = 0; i != indices.size(); ++i)
				std::cout << indices[0] << " | ";
			std::cout << "valid\n";
			validIndices.push_back(indices);
		}
		else
		{
			for (size_t i = 0; i != indices.size(); ++i)
				std::cout << indices[0] << " | ";
			std::cout << "invalid\n";
		}

		++indices[0];
	}

	if (validIndices.size() == 0)
		Error::Report("No valid Device Indices");

	// Get deviceQueueCreateInfo
	std::vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfo;
	std::vector<std::vector<float>> priorities;

	for (size_t iValid = 0; iValid != validIndices[validIndexPick].size(); ++iValid)
	{
		bool found = false;
		for (size_t iInfo = 0; iInfo != deviceQueueCreateInfo.size(); ++iInfo)
		{
			if (validIndices[validIndexPick][iValid] == deviceQueueCreateInfo[iInfo].queueFamilyIndex)
			{
				found = true;
				deviceQueueCreateInfo[iInfo].queueCount += (uint32_t)_queueFamilyRequirements[iValid].priorities.size();

				for (size_t iPriorities = 0; iPriorities != _queueFamilyRequirements[iValid].priorities.size(); ++iPriorities)
				{
					priorities[iInfo].push_back(_queueFamilyRequirements[iValid].priorities[iPriorities]);
				}

				deviceQueueCreateInfo[iInfo].pQueuePriorities = priorities[iInfo].data();
			}
		}

		if (found == false)
		{
			deviceQueueCreateInfo.push_back({});
			deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].pNext = nullptr;
			deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].flags = VK_RESERVED_FOR_FUTURE_USE;
			deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].queueFamilyIndex = validIndices[validIndexPick][iValid];
			deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].queueCount = (uint32_t)_queueFamilyRequirements[iValid].priorities.size();

			priorities.push_back({});
			for (size_t iPriorities = 0; iPriorities != _queueFamilyRequirements[iValid].priorities.size(); ++iPriorities)
				priorities[priorities.size() - 1].push_back(_queueFamilyRequirements[iValid].priorities[iPriorities]);

			deviceQueueCreateInfo[deviceQueueCreateInfo.size() - 1].pQueuePriorities = priorities[priorities.size() - 1].data();
		}
	}

	for (size_t iInfo = 0; iInfo != deviceQueueCreateInfo.size(); ++iInfo)
	{
		deviceQueueCreateInfo[iInfo].queueCount = (uint32_t)priorities[iInfo].size();
		deviceQueueCreateInfo[iInfo].pQueuePriorities = priorities[iInfo].data();
	}

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.pNext = nullptr;
	deviceCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
	deviceCreateInfo.queueCreateInfoCount = (uint32_t)deviceQueueCreateInfo.size();
	deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfo.data();
	deviceCreateInfo.enabledLayerCount = 0; // deprecated
	deviceCreateInfo.ppEnabledLayerNames = nullptr; // deprecated
	deviceCreateInfo.enabledExtensionCount = (uint32_t)_deviceExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = _deviceExtensions.data();
	deviceCreateInfo.pEnabledFeatures = &_features;
	VulkanCommon::CreateDevice(device.physicalDevice->handle, &deviceCreateInfo, device.device);

	/// QUEUE
	device.queueGroups.resize(deviceQueueCreateInfo.size());
	for (size_t iGroup = 0; iGroup != device.queueGroups.size(); ++iGroup)
	{
		device.queueGroups[iGroup].familyIndex = deviceQueueCreateInfo[iGroup].queueFamilyIndex;
		device.queueGroups[iGroup].flags = device.physicalDevice->queueFamilyProperties[device.queueGroups[iGroup].familyIndex].queueFlags;
		device.queueGroups[iGroup].presentability = device.physicalDevice->queueFamilyPresentability[device.queueGroups[iGroup].familyIndex];
		device.queueGroups[iGroup].priority = priorities[device.queueGroups[iGroup].familyIndex];
		device.queueGroups[iGroup].queues.resize(device.queueGroups[iGroup].priority.size());

		for (size_t iQueue = 0; iQueue != device.queueGroups[iGroup].queues.size(); ++iQueue)
			vkGetDeviceQueue(device.device, device.queueGroups[iGroup].familyIndex, (uint32_t)iQueue, &device.queueGroups[iGroup].queues[iQueue]);
	}

	/// COMMAND POOL
	VkCommandPoolCreateInfo commandPoolCreateInfo;
	commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolCreateInfo.pNext = nullptr;
	commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	for (size_t iGroup = 0; iGroup != device.queueGroups.size(); ++iGroup)
	{
		commandPoolCreateInfo.queueFamilyIndex = device.queueGroups[iGroup].familyIndex;
		VulkanCommon::CreateCommandPool(device.device, &commandPoolCreateInfo, &device.queueGroups[iGroup].commandPool);
	}

	device.graphicsCommandPool = device.queueGroups[graphicsGroupID].commandPool;
	device.graphicsQueue = device.queueGroups[graphicsGroupID].queues[graphicsQueueID];
}

void Renderer::InitPresentation()
{
	// Render completion fence
	VkFenceCreateInfo fenceCreateInfo;
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.pNext = nullptr;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	VulkanCommon::CreateFence(device.device, &fenceCreateInfo, &presentation.renderDone);

	// Submission commandBuffer
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = device.graphicsCommandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	commandBufferAllocateInfo.commandBufferCount = 1;
	VulkanCommon::AllocateCommandBuffers(device.device, &commandBufferAllocateInfo, &presentation.submitCommandBuffer);

	// Graphics submission queue
	presentation.renderQueue = device.graphicsQueue;

	// Swapchain
	uint32_t targetImageCount = 3;
	std::vector<VkCompositeAlphaFlagBitsKHR> preferedCompositeAlphas = { VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR, VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR };
	std::vector<VkPresentModeKHR> preferedPresentModes = { VK_PRESENT_MODE_MAILBOX_KHR, VK_PRESENT_MODE_FIFO_KHR };

	for (size_t i = 0; i != instance.surfaces.size(); ++i)
	{
		presentation.swapchainMap[instance.surfaces[i].GetName()] = {};

		presentation.swapchainMap[instance.surfaces[i].GetName()].Init(device.device, device.physicalDevice, instance.surfaces[i].GetHandle(), targetImageCount, preferedCompositeAlphas, preferedPresentModes, device.graphicsCommandPool);
	}
}

void Renderer::InitCompute()
{
	VkCommandBufferAllocateInfo commandBufferAllocateInfo;
	commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	commandBufferAllocateInfo.pNext = nullptr;
	commandBufferAllocateInfo.commandPool = device.graphicsCommandPool;
	commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	commandBufferAllocateInfo.commandBufferCount = 1;
	VulkanCommon::AllocateCommandBuffers(device.device, &commandBufferAllocateInfo, &compute.commandBuffer);
}

void Renderer::InitMemory()
{
	size_t ringCount = 8;
	size_t ateroidPerRing = 3500;

	/// Setup structure definitions
	memory.instanceStruct.SetDatas({
		//Structure::Data::Get("Position3", Structure::DATATYPE::FLOAT, 3),
		//Structure::Data::Get("Padding1", Structure::DATATYPE::FLOAT, 1),

		Structure::Data::Get("Position3", Structure::DATATYPE::FLOAT, 3),
		Structure::Data::Get("Color3", Structure::DATATYPE::FLOAT, 3),
		Structure::Data::Get("Scale", Structure::DATATYPE::FLOAT, 1),
		Structure::Data::Get("Eulers", Structure::DATATYPE::FLOAT, 3),
		Structure::Data::Get("Padding2", Structure::DATATYPE::FLOAT, 2),
	});
	memory.instanceStruct.SetCount(2 + ringCount * ateroidPerRing * 40);

	memory.speedStruct.SetDatas({
		Structure::Data::Get("Speed3", Structure::DATATYPE::FLOAT, 3),
		Structure::Data::Get("Padding1", Structure::DATATYPE::FLOAT, 1),
	});
	memory.speedStruct.SetCount(memory.instanceStruct.GetCount());

	memory.viewProjectionStruct.SetDatas({
		Structure::Data::Get("View", Structure::DATATYPE::FLOAT, 16),
		Structure::Data::Get("Projection", Structure::DATATYPE::FLOAT, 16),
	});
	memory.viewProjectionStruct.SetCount(1);

	memory.deltaTimeStruct.SetDatas({
		Structure::Data::Get("DeltaTime", Structure::DATATYPE::FLOAT, 1),
		Structure::Data::Get("Gravity", Structure::DATATYPE::FLOAT, 1),
	});
	memory.deltaTimeStruct.SetCount(1);

	/// Init transfer
	memory.transferManager.Init(device.device, device.physicalDevice, device.graphicsCommandPool, device.graphicsQueue, 0);

	/// Load data
	// Mesh info
	std::string typeNames[] = { "Pos3", "UvNormal" };
	struct Meshes
	{
		std::string name;
		Loader::MeshData mesh;

		static inline Meshes Get(std::string _name, Loader::MeshData _mesh)
		{
			return { _name, _mesh };
		}
	};
	std::vector<const char*> filenames = {
		"../../Data/Models/Saturn.fbx",
		"../../Data/Models/Ico.fbx",
		"../../Data/Models/CubeMap.fbx",
	};
	std::vector<Meshes> meshesData = {
		Meshes::Get("Saturn",{}),
		Meshes::Get("Ico",{}),
		Meshes::Get("CubeMap",{}),
	};

	// Mesh data
	for (size_t i = 0; i != meshesData.size(); ++i)
	{
		meshesData[i].mesh = Loader::LoadMeshData(filenames[i], { Loader::MeshData::POS3, Loader::MeshData::UV_NORMAL });
	}

	// Load RGBA8 data
	struct Textures
	{
		std::string name;
		Loader::DataImage image;

		static inline Textures Get(std::string _name, Loader::DataImage _image)
		{
			return { _name, _image };
		}
	};
	std::vector<Textures> rgba8Textures = {
		Textures::Get("RockColor",{}),
		Textures::Get("SaturnColor",{}),
		Textures::Get("CubeMapColor",{}),
		Textures::Get("FontColor",{}),
	};
	Loader::LoadTGA("../../Data/Images/RockColor.tga", rgba8Textures[0].image);
	Loader::LoadTGA("../../Data/Images/SaturnColor.tga", rgba8Textures[1].image);
	Loader::LoadTGA("../../Data/Images/CubeMapColor.tga", rgba8Textures[2].image);
	Loader::LoadTGA("../../Data/Images/Consolas2.tga", rgba8Textures[3].image);

	/// Setup data
	std::mt19937 rGenerator(1);
	std::uniform_real_distribution<float> rDistribution(-1.0f, 1.0f);

	// Depth info
	struct DepthImageProps
	{
		std::string name;
		uint32_t width;
		uint32_t height;
	
		static DepthImageProps Get(std::string _name, uint32_t _width, uint32_t _height)
		{
			DepthImageProps depthImageProps;
			depthImageProps.name = _name;
			depthImageProps.width = _width;
			depthImageProps.height = _height;
			return depthImageProps;
		}
	};
	std::vector<DepthImageProps> depthProps;
	
	for (std::map<std::string, Swapchain>::iterator iter = presentation.swapchainMap.begin(); iter != presentation.swapchainMap.end(); ++iter)
		depthProps.push_back(DepthImageProps::Get(iter->first + "Depth", iter->second.extent.width, iter->second.extent.height));

	// Instance buffer
	StructuredBuffer instanceStructBuffer;
	instanceStructBuffer.Init(memory.instanceStruct);

	// Speed buffer
	StructuredBuffer speedStructBuffer;
	speedStructBuffer.Init(memory.speedStruct);

	// angles
	std::vector<float> angles(memory.instanceStruct.GetCount());
	for (size_t i = 0; i != angles.size(); ++i)
	{
		angles[i] = (rDistribution(rGenerator) + 1.0f) * MATH_PI;
	}

	// rings
	struct RingInfo
	{
		Math3D::Vec3 color;
		float distance;
		float force;

		static inline RingInfo Get(Math3D::Vec3 _color, float _distance, float _force)
		{
			return { _color, _distance, _force };
		}
	};
	std::vector<RingInfo> ringInfos(8);
	ringInfos[0] = RingInfo::Get({ 2.5f, 1.2f, 0.75f }, 70.0f, 0.2685f);
	ringInfos[1] = RingInfo::Get({ 0.0f, 1.5f, 0.75f }, 65.0f, 0.28f);
	ringInfos[2] = RingInfo::Get({ 1.0f, 1.25f, 0.625f }, 60.0f, 0.29f);
	ringInfos[3] = RingInfo::Get({ 2.0f, 1.0f, 0.0f }, 55.0f, 0.31f);
	ringInfos[4] = RingInfo::Get({ 2.0f, 0.75f, 0.0f }, 50.0f, 0.32f);
	ringInfos[5] = RingInfo::Get({ 0.0f, 0.0f, 2.0f }, 45.0f, 0.33f);
	ringInfos[6] = RingInfo::Get({ 1.25f, 1.25f, 2.0f }, 40.0f, 0.35f);
	ringInfos[7] = RingInfo::Get({ 0.0f, 0.0f, 2.0f }, 35.0f, 0.38f);

	Math3D::Vec3* AddrsPosition3;
	Math3D::Vec3* AddrsColor3;
	float* AddrsScale;
	Math3D::Vec3* AddrsEulers;
	Math3D::Vec3* AddrsSpeed3;

	for (size_t i = 0; i != ringCount; ++i)
	{
		for (size_t j = 0; j != ateroidPerRing; ++j)
		{
			size_t index = i * ateroidPerRing + j;

			AddrsPosition3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(index, "Position3"));
			AddrsColor3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(index, "Color3"));
			AddrsScale = (float*)(instanceStructBuffer.GetDataAddress(index, "Scale"));

			float distance = ringInfos[i].distance + rDistribution(rGenerator) * 1.5f;
			*AddrsPosition3 = { sinf(angles[index]) * distance, cosf(angles[index]) * distance, rDistribution(rGenerator) * 0.0f};
			*AddrsColor3 = ringInfos[i].color;
			*AddrsScale = abs(rDistribution(rGenerator)) * 0.2f + 0.05f;

			float force = ringInfos[i].force + rDistribution(rGenerator) * 0.001f;
			AddrsSpeed3 = (Math3D::Vec3*)(speedStructBuffer.GetDataAddress(index, "Speed3"));
			*AddrsSpeed3 = { sinf(angles[index] + MATH_PI * 0.5f) * force, cosf(angles[index] + MATH_PI * 0.5f) * force, 0.0025f * abs(abs(i-4.0f) - 4) * rDistribution(rGenerator) };
		}
	}

	AddrsPosition3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(0, "Position3"));
	AddrsColor3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(0, "Color3"));
	AddrsScale = (float*)(instanceStructBuffer.GetDataAddress(0, "Scale"));
	AddrsEulers = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(0, "Eulers"));
	*AddrsPosition3 = { 0.0f, 0.0f, 0.0f, };
	*AddrsColor3 = (Math3D::Vec3({ 1.0f, 0.5f, 0.25f, }) * 1.0f);
	*AddrsScale = 25.0f;
	*AddrsEulers = { 0.0f , 0.0f , 0.0f };

	AddrsPosition3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(1, "Position3"));
	AddrsColor3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(1, "Color3"));
	AddrsScale = (float*)(instanceStructBuffer.GetDataAddress(1, "Scale"));
	AddrsEulers = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(1, "Eulers"));
	*AddrsPosition3 = { 0.0f, 0.0f, 0.0f, };
	*AddrsColor3 = (Math3D::Vec3({ 1.0f, 1.0f, 0.8f, }) * 0.5f);
	*AddrsScale = -800.0f;
	*AddrsEulers = { 0.0f , 0.0f , 0.0f };

	AddrsPosition3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(2, "Position3"));
	AddrsColor3 = (Math3D::Vec3*)(instanceStructBuffer.GetDataAddress(2, "Color3"));
	AddrsScale = (float*)(instanceStructBuffer.GetDataAddress(2, "Scale"));
	AddrsSpeed3 = (Math3D::Vec3*)(speedStructBuffer.GetDataAddress(2, "Speed3"));
	float distance = 70.0f;
	*AddrsPosition3 = { sinf(angles[2]) * distance, cosf(angles[2]) * distance, -50.0f };
	*AddrsColor3 = (Math3D::Vec3({ 1.0f, 1.0f, 1.0f, }) * 1.5f);
	*AddrsScale = 2.0f;
	*AddrsSpeed3 = *AddrsSpeed3 * 0.88f;

	// View projection buffer
	StructuredBuffer viewProjectionStructBuffer;
	viewProjectionStructBuffer.Init(memory.viewProjectionStruct);

	Camera camera;
	camera.position = { 0.0f, 0.0f, 0.0f };
	camera.rotation = { 0.0f, 0.0f, 0.0f };
	camera.fov = 45.0f;
	camera.aspect = 800.0f / 600.0f;
	camera.nearV = 0.1f;
	camera.farV = 1000.0f;
	camera.speed = 10.0f;

	Math3D::Mat4* AddrsView = (Math3D::Mat4*)(viewProjectionStructBuffer.GetDataAddress(0, "View"));
	Math3D::Mat4* AddrsProjection = (Math3D::Mat4*)(viewProjectionStructBuffer.GetDataAddress(0, "Projection"));

	*AddrsView = camera.GetLookAt();
	*AddrsProjection = camera.GetProjectionMatrix();

	// DeltaTime buffer
	StructuredBuffer deltaTimeStructBuffer;
	deltaTimeStructBuffer.Init(memory.deltaTimeStruct);

	float* AddrsDeltaTime = (float*)(deltaTimeStructBuffer.GetDataAddress(0, "DeltaTime"));
	float* AddrsGravity = (float*)(deltaTimeStructBuffer.GetDataAddress(0, "Gravity"));

	*AddrsDeltaTime = 0.000f;
	*AddrsGravity = 5.000f;

	/// Get GPU buffers sizes
	size_t vertexBufferSize = 0;
	size_t indexBufferSize = 0;
	std::vector<VkImage> rgba8Images;
	std::vector<VkMemoryRequirements> rgba8MemoryRequirements;
	size_t rgba8BufferSize = 0;
	std::vector<VkImage> depthImages;
	std::vector<VkMemoryRequirements> depthMemoryRequirements;
	size_t depthBufferSize = 0;
	size_t computeVertexBufferSize = 0;
	size_t computeBufferSize = 0;
	size_t uniformBufferSize = 0;

	// Add Mesh sizes
	for (size_t iMesh = 0; iMesh != meshesData.size(); ++iMesh)
	{
		for (size_t i = 0; i != meshesData[iMesh].mesh.vertices.size(); ++i)
			vertexBufferSize += meshesData[iMesh].mesh.vertices[i].size;

		indexBufferSize += meshesData[iMesh].mesh.indexSize;
	}

	// Add RGBA8 sizes
	rgba8Images.resize(rgba8Textures.size());
	rgba8MemoryRequirements.resize(rgba8Textures.size());
	for (size_t iRGBA8 = 0; iRGBA8 != rgba8Textures.size(); ++iRGBA8)
	{
		// Create image handle
		VulkanCommon::CreateImage(
			device.device,
			&VulkanCommon::GetVkImageCreateInfo(
				rgba8Textures[iRGBA8].image.width,
				rgba8Textures[iRGBA8].image.height,
				device.physicalDevice->depthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
			&rgba8Images[iRGBA8]);

		// Validate memory requirements
		vkGetImageMemoryRequirements(device.device, rgba8Images[iRGBA8], &rgba8MemoryRequirements[iRGBA8]);

		// Add to size
		rgba8BufferSize += rgba8MemoryRequirements[iRGBA8].size;
	}
	rgba8BufferSize += 8192; // Add some alignment padding

	// Add Depth sizes
	depthImages.resize(depthProps.size());
	depthMemoryRequirements.resize(depthProps.size());
	for (size_t iDepth = 0; iDepth != depthProps.size(); ++iDepth)
	{
		// Create image handle
		VulkanCommon::CreateImage(
			device.device,
			&VulkanCommon::GetVkImageCreateInfo(
				depthProps[iDepth].width,
				depthProps[iDepth].height,
				device.physicalDevice->depthFormat,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT),
			&depthImages[iDepth]);

		// Validate memory requirements
		vkGetImageMemoryRequirements(device.device, depthImages[iDepth], &depthMemoryRequirements[iDepth]);

		// Add to size
		depthBufferSize += depthMemoryRequirements[iDepth].size;
	}
	depthBufferSize += 8192; // Add some alignment padding

	// Add Instance size
	computeVertexBufferSize += instanceStructBuffer.dataSize;

	// Add Speed size
	computeBufferSize += speedStructBuffer.dataSize;

	// Add ViewProjection size
	uniformBufferSize += MemoryMap::GetAlignedSize(viewProjectionStructBuffer.dataSize, device.physicalDevice->properties.limits.minUniformBufferOffsetAlignment);

	// Add deltaTime size
	uniformBufferSize += MemoryMap::GetAlignedSize(deltaTimeStructBuffer.dataSize, device.physicalDevice->properties.limits.minUniformBufferOffsetAlignment);

	/// Init GPU memories
	memory.vertexBuffer.Init(device.device, device.physicalDevice,
		vertexBufferSize,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	memory.indexBuffer.Init(device.device, device.physicalDevice,
		indexBufferSize,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	memory.RGBA8Images.Init(device.device, device.physicalDevice,
		rgba8BufferSize,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
	memory.depthImages.Init(device.device, device.physicalDevice,
		depthBufferSize,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		device.physicalDevice->depthFormat,
		VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
	memory.computeVertexBuffer.Init(device.device, device.physicalDevice,
		computeVertexBufferSize,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	memory.computeBuffer.Init(device.device, device.physicalDevice,
		computeBufferSize,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
	memory.uniformBuffer.Init(device.device, device.physicalDevice,
		uniformBufferSize,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

	/// Map pools
	MemoryMap::PoolNode* vertexPool = memory.vertexBuffer.AllocatePool("Pool", vertexBufferSize, 0, nullptr);
	MemoryMap::PoolNode* indexPool = memory.indexBuffer.AllocatePool("Pool", indexBufferSize, 0, nullptr);
	MemoryMap::PoolNode* rgba8Pool = memory.RGBA8Images.AllocatePool("Pool", rgba8BufferSize, memory.RGBA8Images.alignment, nullptr);
	MemoryMap::PoolNode* depthPool = memory.depthImages.AllocatePool("Pool", depthBufferSize, memory.depthImages.alignment, nullptr);
	MemoryMap::PoolNode* computeVertexPool = memory.computeVertexBuffer.AllocatePool("Pool", computeVertexBufferSize, 0, nullptr);
	MemoryMap::PoolNode* computePool = memory.computeBuffer.AllocatePool("Pool", computeBufferSize, 0, nullptr);
	MemoryMap::PoolNode* uniformPool = memory.uniformBuffer.AllocatePool("Pool", uniformBufferSize, device.physicalDevice->properties.limits.minUniformBufferOffsetAlignment, nullptr);

	/// Map buffer, allocate & transfer
	// Mesh
	for (size_t iMesh = 0; iMesh != meshesData.size(); ++iMesh)
	{
		for (size_t i = 0; i != meshesData[iMesh].mesh.vertices.size(); ++i)
		{
			// Do allocation
			MemoryMap::BufferNode* vertexBuffer = vertexPool->AllocateBuffer(
				(std::string(meshesData[iMesh].name) + typeNames[i]).c_str(),
				meshesData[iMesh].mesh.vertices[i].size,
				nullptr);

			// Transfer data
			memory.transferManager.CpuTransfer(meshesData[iMesh].mesh.vertices[i].data, meshesData[iMesh].mesh.vertices[i].size, memory.vertexBuffer.handle, vertexBuffer->memoryInfo.offset);
			vertexBuffer->state = MemoryMap::MEMORY_STATE::VALID;
		}

		// Do allocation
		MemoryMap::BufferNode* indexBuffer = indexPool->AllocateBuffer(
			(std::string(meshesData[iMesh].name) + "Index").c_str(),
			meshesData[iMesh].mesh.indexSize,
			MemoryMap::GetNewT<Memory::IndexUserdata>(Memory::IndexUserdata::Get((VkIndexType)(
				VK_INDEX_TYPE_UINT16 * (meshesData[iMesh].mesh.indexType == Structure::DATATYPE::UINT16) +
				VK_INDEX_TYPE_UINT32 * (meshesData[iMesh].mesh.indexType == Structure::DATATYPE::UINT32)),
				meshesData[iMesh].mesh.indexCount)));

		// Transfer data
		memory.transferManager.CpuTransfer(meshesData[iMesh].mesh.indexData, meshesData[iMesh].mesh.indexSize, memory.indexBuffer.handle, indexBuffer->memoryInfo.offset);
		indexBuffer->state = MemoryMap::MEMORY_STATE::VALID;
	}

	// RGBA8
	// Create image, do allocation, transfer & create view
	for (size_t iRGBA8 = 0; iRGBA8 != rgba8Textures.size(); ++iRGBA8)
	{
		// Create image handle
		VkImage imageHandle;
		VulkanCommon::CreateImage(
			device.device,
			&VulkanCommon::GetVkImageCreateInfo(
				rgba8Textures[iRGBA8].image.width,
				rgba8Textures[iRGBA8].image.height,
				memory.RGBA8Images.format,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT),
			&imageHandle);

		// Validate memory requirements
		VkMemoryRequirements memoryRequirements;
		vkGetImageMemoryRequirements(device.device, imageHandle, &memoryRequirements);
		if ((memoryRequirements.memoryTypeBits & (1 << memory.RGBA8Images.memoryTypeIndex)) == 0)
			Error::Report("Image not compatible with memory");

		// Allocate
		MemoryMap::BufferNode* rgba8Buffer = rgba8Pool->AllocateBuffer(
			rgba8Textures[iRGBA8].name.c_str(),
			rgba8MemoryRequirements[iRGBA8].size,
			MemoryMap::GetNewT<Memory::ImageUserdata>(Memory::ImageUserdata::Get(
				imageHandle,
				{},
				rgba8Textures[iRGBA8].image.dataSize)));

		// Bind image to memory
		VulkanCommon::BindImageMemory(device.device, imageHandle, memory.RGBA8Images.textureMemory, rgba8Buffer->memoryInfo.offset);

		// Create image view
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		imageViewCreateInfo.image = imageHandle;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = memory.RGBA8Images.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		VulkanCommon::CreateImageView(device.device, &imageViewCreateInfo, &((Memory::ImageUserdata*)rgba8Buffer->userdata)->view);

		// Transfer data
		memory.transferManager.CpuTransfer(rgba8Textures[iRGBA8].image.data, rgba8Textures[iRGBA8].image.dataSize, imageHandle, rgba8Buffer->memoryInfo.offset, { rgba8Textures[iRGBA8].image.width, rgba8Textures[iRGBA8].image.height, 1 });
		rgba8Buffer->state = MemoryMap::MEMORY_STATE::VALID;
	}

	// Depth
	memory.transferManager.WaitResetTransfer();
	VkCommandBuffer commandBuffer = memory.transferManager.BeginCommandBuffer();

	for (size_t iDepth = 0; iDepth != depthProps.size(); ++iDepth)
	{
		// Allocate
		MemoryMap::BufferNode* depthBuffer = depthPool->AllocateBuffer(
			depthProps[iDepth].name.c_str(),
			depthMemoryRequirements[iDepth].size,
			MemoryMap::GetNewT<Memory::ImageUserdata>(Memory::ImageUserdata::Get(
				depthImages[iDepth],
				{},
				0)));

		// Bind image to memory
		VulkanCommon::BindImageMemory(device.device, depthImages[iDepth], memory.depthImages.textureMemory, depthBuffer->memoryInfo.offset);

		// Create image view
		VkImageViewCreateInfo imageViewCreateInfo;
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.pNext = nullptr;
		imageViewCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		imageViewCreateInfo.image = depthImages[iDepth];
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format = memory.depthImages.format;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		VulkanCommon::CreateImageView(device.device, &imageViewCreateInfo, &((Memory::ImageUserdata*)depthBuffer->userdata)->view);

		// Record transition depth layout
		VkImageMemoryBarrier imageMemoryBarrier;
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;
		imageMemoryBarrier.srcAccessMask = 0;
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;// VK_IMAGE_LAYOUT_PREINITIALIZED;
		imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = depthImages[iDepth];
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = 1;
		vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT, 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}

	memory.transferManager.EndCommandBuffer();
	memory.transferManager.SubmitCommandBuffer();

	// Instance
	MemoryMap::BufferNode* computeVertexStructureBuffer = computeVertexPool->AllocateBuffer(
		"Instance",
		instanceStructBuffer.dataSize,
		MemoryMap::GetNewT<Memory::InstanceUserdata>(Memory::InstanceUserdata::Get((uint32_t)instanceStructBuffer.structure.GetCount())));

	memory.transferManager.CpuTransfer(instanceStructBuffer.data, instanceStructBuffer.dataSize, memory.computeVertexBuffer.handle, computeVertexStructureBuffer->memoryInfo.offset);
	computeVertexStructureBuffer->state = MemoryMap::MEMORY_STATE::VALID;

	// Speed
	MemoryMap::BufferNode* computeStructureBuffer = computePool->AllocateBuffer(
		"Rock01Speed",
		speedStructBuffer.dataSize,
		MemoryMap::GetNewT<Memory::InstanceUserdata>(Memory::InstanceUserdata::Get((uint32_t)speedStructBuffer.structure.GetCount())));

	memory.transferManager.CpuTransfer(speedStructBuffer.data, speedStructBuffer.dataSize, memory.computeBuffer.handle, computeStructureBuffer->memoryInfo.offset);
	computeStructureBuffer->state = MemoryMap::MEMORY_STATE::VALID;

	// ViewProjection
	MemoryMap::BufferNode* uniformBuffer = uniformPool->AllocateBuffer(
		"ViewProjection",
		viewProjectionStructBuffer.dataSize,
		nullptr);

	memory.transferManager.CpuTransfer(viewProjectionStructBuffer.data, viewProjectionStructBuffer.dataSize, memory.uniformBuffer.handle, uniformBuffer->memoryInfo.offset);
	uniformBuffer->state = MemoryMap::MEMORY_STATE::VALID;

	// DeltaTime
	MemoryMap::BufferNode* deltaTimeBuffer = uniformPool->AllocateBuffer(
		"DeltaTime",
		deltaTimeStructBuffer.dataSize,
		nullptr);

	memory.transferManager.CpuTransfer(deltaTimeStructBuffer.data, deltaTimeStructBuffer.dataSize, memory.uniformBuffer.handle, deltaTimeBuffer->memoryInfo.offset);
	deltaTimeBuffer->state = MemoryMap::MEMORY_STATE::VALID;

	// Free data
	for(size_t i = 0; i != meshesData.size(); ++i)
		Loader::UnloadMeshData(meshesData[i].mesh);
	for (size_t i = 0; i != rgba8Textures.size(); ++i)
		Loader::UnloadTGA(rgba8Textures[i].image);
	instanceStructBuffer.Clear();
	speedStructBuffer.Clear();
	viewProjectionStructBuffer.Clear();
	deltaTimeStructBuffer.Clear();
}

void Renderer::InitFramebuffers()
{
	// Create framebuffers for each swapchain
	for (std::map<std::string, Swapchain>::iterator iter = presentation.swapchainMap.begin(); iter != presentation.swapchainMap.end(); ++iter)
	{
		// Create window framebuffers
		VkFramebufferCreateInfo framebufferCreateInfo;
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.pNext = nullptr;
		framebufferCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		framebufferCreateInfo.renderPass = iter->second.renderPass;
		framebufferCreateInfo.width = iter->second.extent.width;
		framebufferCreateInfo.height = iter->second.extent.height;
		framebufferCreateInfo.layers = 1;

		iter->second.framebuffers.resize(iter->second.views.size());
		for (size_t iFramebuffer = 0; iFramebuffer != iter->second.framebuffers.size(); ++iFramebuffer)
		{
			MemoryMap::PoolNode* pool = memory.depthImages.memoryMap.GetPool("Pool");
			MemoryMap::BufferNode* buffer = pool->GetBuffer("WindowDepth");

			std::vector<VkImageView> attachments;
			attachments = { iter->second.views[iFramebuffer], ((Memory::ImageUserdata*)buffer->userdata)->view };

			framebufferCreateInfo.attachmentCount = (uint32_t)attachments.size();
			framebufferCreateInfo.pAttachments = attachments.data();

			VulkanCommon::CreateFramebuffer(device.device, &framebufferCreateInfo, &iter->second.framebuffers[iFramebuffer]);
		}
	}
}

void Renderer::InitSamplers()
{
	VkSamplerCreateInfo samplerCreateInfo;
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.pNext = nullptr;
	samplerCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerCreateInfo.mipLodBias = 0.0f;
	samplerCreateInfo.anisotropyEnable = VK_TRUE;
	samplerCreateInfo.maxAnisotropy = 16;
	samplerCreateInfo.compareEnable = VK_FALSE;
	samplerCreateInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerCreateInfo.minLod = 0.0f;
	samplerCreateInfo.maxLod = 0.0f;
	samplerCreateInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;

	VkSampler vkSampler;
	VulkanCommon::CreateSampler(device.device, &samplerCreateInfo, &vkSampler);
	sampler.samplers["Sampler"] = vkSampler;
}

void Renderer::InitDescriptos()
{
	/// Layouts
	{
		// Vertex_1Uniform
		{
			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
				VulkanCommon::GetVkDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr),
			};

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = VK_FLAGS_NONE;
			descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
			descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();

			VkDescriptorSetLayout descriptorSetLayout;
			VulkanCommon::CreateDescriptorSetLayout(device.device, &descriptorSetLayoutCreateInfo, &descriptorSetLayout);

			descriptor.descriptorSetLayouts["Vertex_1Uniform"] = descriptorSetLayout;
		}

		// Fragment_1Sampler
		{
			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
				VulkanCommon::GetVkDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr),
			};

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = VK_FLAGS_NONE;
			descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
			descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();

			VkDescriptorSetLayout descriptorSetLayout;
			VulkanCommon::CreateDescriptorSetLayout(device.device, &descriptorSetLayoutCreateInfo, &descriptorSetLayout);

			descriptor.descriptorSetLayouts["Fragment_1Sampler"] = descriptorSetLayout;
		}

		// Compute_1Uniform2Storage
		{
			std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
				VulkanCommon::GetVkDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr),
				VulkanCommon::GetVkDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr),
				VulkanCommon::GetVkDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_COMPUTE_BIT, nullptr),
			};

			VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.pNext = nullptr;
			descriptorSetLayoutCreateInfo.flags = VK_FLAGS_NONE;
			descriptorSetLayoutCreateInfo.bindingCount = (uint32_t)setLayoutBindings.size();
			descriptorSetLayoutCreateInfo.pBindings = setLayoutBindings.data();

			VkDescriptorSetLayout descriptorSetLayout;
			VulkanCommon::CreateDescriptorSetLayout(device.device, &descriptorSetLayoutCreateInfo, &descriptorSetLayout);

			descriptor.descriptorSetLayouts["Compute_1Uniform2Storage"] = descriptorSetLayout;
		}
	}

	/// Pool
	{
		std::vector<VkDescriptorPoolSize> descriptorPoolSizes = {
			VulkanCommon::GetVkDescriptorPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2),
			VulkanCommon::GetVkDescriptorPoolSize(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4),
			VulkanCommon::GetVkDescriptorPoolSize(VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 2),
		};

		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo;
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.pNext = nullptr;
		descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		descriptorPoolCreateInfo.maxSets = 6;
		descriptorPoolCreateInfo.poolSizeCount = (uint32_t)descriptorPoolSizes.size();
		descriptorPoolCreateInfo.pPoolSizes = descriptorPoolSizes.data();

		VkDescriptorPool descriptorPool;
		VulkanCommon::CreateDescriptorPool(device.device, &descriptorPoolCreateInfo, &descriptorPool);

		descriptor.descriptorPools["Pool"] = descriptorPool;
	}

	/// Sets
	{
		// ViewProjection
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = descriptor.descriptorPools["Pool"];
			descriptorSetAllocateInfo.descriptorSetCount = 1;
			descriptorSetAllocateInfo.pSetLayouts = &descriptor.descriptorSetLayouts["Vertex_1Uniform"];

			VkDescriptorSet set;
			VulkanCommon::AllocateDescriptorSets(device.device, &descriptorSetAllocateInfo, &set);

			descriptor.descriptorSets["ViewProjection"] = set;
		}

		// Texture
		{
			VkDescriptorSetLayout pSetLayouts[] = {
				descriptor.descriptorSetLayouts["Fragment_1Sampler"],
				descriptor.descriptorSetLayouts["Fragment_1Sampler"],
				descriptor.descriptorSetLayouts["Fragment_1Sampler"],
				descriptor.descriptorSetLayouts["Fragment_1Sampler"],
			};
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = descriptor.descriptorPools["Pool"];
			descriptorSetAllocateInfo.descriptorSetCount = 4;
			descriptorSetAllocateInfo.pSetLayouts = pSetLayouts;

			VkDescriptorSet sets[4];
			VulkanCommon::AllocateDescriptorSets(device.device, &descriptorSetAllocateInfo, sets);

			descriptor.descriptorSets["Asteroid"] = sets[0];
			descriptor.descriptorSets["Saturn"] = sets[1];
			descriptor.descriptorSets["CubeMap"] = sets[2];
			descriptor.descriptorSets["Font"] = sets[3];
		}

		// Instance
		{
			VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
			descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			descriptorSetAllocateInfo.pNext = nullptr;
			descriptorSetAllocateInfo.descriptorPool = descriptor.descriptorPools["Pool"];
			descriptorSetAllocateInfo.descriptorSetCount = 1;
			descriptorSetAllocateInfo.pSetLayouts = &descriptor.descriptorSetLayouts["Compute_1Uniform2Storage"];

			VkDescriptorSet set;
			VulkanCommon::AllocateDescriptorSets(device.device, &descriptorSetAllocateInfo, &set);

			descriptor.descriptorSets["Instance"] = set;
		}
	}

	/// Update sets
	{
		std::vector<VkWriteDescriptorSet> writeDescriptorSets(6);

		MemoryMap::BufferNode* buffer;

		// ViewProjection
		buffer = memory.uniformBuffer.memoryMap.GetPool("Pool")->GetBuffer("ViewProjection");
		VkDescriptorBufferInfo viewProjectionDescriptorBufferInfo;
		viewProjectionDescriptorBufferInfo.buffer = memory.uniformBuffer.handle;
		viewProjectionDescriptorBufferInfo.offset = buffer->memoryInfo.offset;
		viewProjectionDescriptorBufferInfo.range = buffer->memoryInfo.size;

		writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[0].pNext = nullptr;
		writeDescriptorSets[0].dstSet = descriptor.descriptorSets["ViewProjection"];
		writeDescriptorSets[0].dstBinding = 0;
		writeDescriptorSets[0].dstArrayElement = 0;
		writeDescriptorSets[0].descriptorCount = 1;
		writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[0].pImageInfo = nullptr;
		writeDescriptorSets[0].pBufferInfo = &viewProjectionDescriptorBufferInfo;
		writeDescriptorSets[0].pTexelBufferView = nullptr;

		// Arteroid
		VkDescriptorImageInfo descriptorImageInfo;
		descriptorImageInfo.sampler = sampler.samplers["Sampler"];
		descriptorImageInfo.imageView = ((Memory::ImageUserdata*)memory.RGBA8Images.memoryMap.GetPool("Pool")->GetBuffer("RockColor")->userdata)->view;
		descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[1].pNext = nullptr;
		writeDescriptorSets[1].dstSet = descriptor.descriptorSets["Asteroid"];
		writeDescriptorSets[1].dstBinding = 1;
		writeDescriptorSets[1].dstArrayElement = 0;
		writeDescriptorSets[1].descriptorCount = 1;
		writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[1].pImageInfo = &descriptorImageInfo;
		writeDescriptorSets[1].pBufferInfo = nullptr;
		writeDescriptorSets[1].pTexelBufferView = nullptr;

		// Saturn
		VkDescriptorImageInfo descriptorImageInfo2;
		descriptorImageInfo2.sampler = sampler.samplers["Sampler"];
		descriptorImageInfo2.imageView = ((Memory::ImageUserdata*)memory.RGBA8Images.memoryMap.GetPool("Pool")->GetBuffer("SaturnColor")->userdata)->view;
		descriptorImageInfo2.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		writeDescriptorSets[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[2].pNext = nullptr;
		writeDescriptorSets[2].dstSet = descriptor.descriptorSets["Saturn"];
		writeDescriptorSets[2].dstBinding = 1;
		writeDescriptorSets[2].dstArrayElement = 0;
		writeDescriptorSets[2].descriptorCount = 1;
		writeDescriptorSets[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[2].pImageInfo = &descriptorImageInfo2;
		writeDescriptorSets[2].pBufferInfo = nullptr;
		writeDescriptorSets[2].pTexelBufferView = nullptr;
		
		// CubeMap
		VkDescriptorImageInfo descriptorImageInfo3;
		descriptorImageInfo3.sampler = sampler.samplers["Sampler"];
		descriptorImageInfo3.imageView = ((Memory::ImageUserdata*)memory.RGBA8Images.memoryMap.GetPool("Pool")->GetBuffer("CubeMapColor")->userdata)->view;
		descriptorImageInfo3.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		writeDescriptorSets[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[3].pNext = nullptr;
		writeDescriptorSets[3].dstSet = descriptor.descriptorSets["CubeMap"];
		writeDescriptorSets[3].dstBinding = 1;
		writeDescriptorSets[3].dstArrayElement = 0;
		writeDescriptorSets[3].descriptorCount = 1;
		writeDescriptorSets[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[3].pImageInfo = &descriptorImageInfo3;
		writeDescriptorSets[3].pBufferInfo = nullptr;
		writeDescriptorSets[3].pTexelBufferView = nullptr;

		// Font
		VkDescriptorImageInfo descriptorImageInfo4;
		descriptorImageInfo4.sampler = sampler.samplers["Sampler"];
		descriptorImageInfo4.imageView = ((Memory::ImageUserdata*)memory.RGBA8Images.memoryMap.GetPool("Pool")->GetBuffer("FontColor")->userdata)->view;
		descriptorImageInfo4.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[4].pNext = nullptr;
		writeDescriptorSets[4].dstSet = descriptor.descriptorSets["Font"];
		writeDescriptorSets[4].dstBinding = 1;
		writeDescriptorSets[4].dstArrayElement = 0;
		writeDescriptorSets[4].descriptorCount = 1;
		writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		writeDescriptorSets[4].pImageInfo = &descriptorImageInfo4;
		writeDescriptorSets[4].pBufferInfo = nullptr;
		writeDescriptorSets[4].pTexelBufferView = nullptr;

		// Instance
		buffer = memory.computeVertexBuffer.memoryMap.GetPool("Pool")->GetBuffer("Instance");
		VkDescriptorBufferInfo instanceDescriptorBufferInfo;
		instanceDescriptorBufferInfo.buffer = memory.computeVertexBuffer.handle;
		instanceDescriptorBufferInfo.offset = buffer->memoryInfo.offset;
		instanceDescriptorBufferInfo.range = buffer->memoryInfo.size;

		buffer = memory.computeBuffer.memoryMap.GetPool("Pool")->GetBuffer("Rock01Speed");
		VkDescriptorBufferInfo speedDescriptorBufferInfo;
		speedDescriptorBufferInfo.buffer = memory.computeBuffer.handle;
		speedDescriptorBufferInfo.offset = buffer->memoryInfo.offset;
		speedDescriptorBufferInfo.range = buffer->memoryInfo.size;

		VkDescriptorBufferInfo descriptorBufferInfos[] = { instanceDescriptorBufferInfo, speedDescriptorBufferInfo };

		writeDescriptorSets[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[4].pNext = nullptr;
		writeDescriptorSets[4].dstSet = descriptor.descriptorSets["Instance"];
		writeDescriptorSets[4].dstBinding = 1;
		writeDescriptorSets[4].dstArrayElement = 0;
		writeDescriptorSets[4].descriptorCount = 2;
		writeDescriptorSets[4].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		writeDescriptorSets[4].pImageInfo = nullptr;
		writeDescriptorSets[4].pBufferInfo = descriptorBufferInfos;
		writeDescriptorSets[4].pTexelBufferView = nullptr;

		// DeltaTime
		buffer = memory.uniformBuffer.memoryMap.GetPool("Pool")->GetBuffer("DeltaTime");
		VkDescriptorBufferInfo deltaTimeDescriptorBufferInfo;
		deltaTimeDescriptorBufferInfo.buffer = memory.uniformBuffer.handle;
		deltaTimeDescriptorBufferInfo.offset = buffer->memoryInfo.offset;
		deltaTimeDescriptorBufferInfo.range = buffer->memoryInfo.size;

		writeDescriptorSets[5].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeDescriptorSets[5].pNext = nullptr;
		writeDescriptorSets[5].dstSet = descriptor.descriptorSets["Instance"];
		writeDescriptorSets[5].dstBinding = 0;
		writeDescriptorSets[5].dstArrayElement = 0;
		writeDescriptorSets[5].descriptorCount = 1;
		writeDescriptorSets[5].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		writeDescriptorSets[5].pImageInfo = nullptr;
		writeDescriptorSets[5].pBufferInfo = &deltaTimeDescriptorBufferInfo;
		writeDescriptorSets[5].pTexelBufferView = nullptr;

		vkUpdateDescriptorSets(device.device, (uint32_t)writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
	}
}

void Renderer::InitShaders()
{
	/// Vert
	{
		std::vector<char> shaderCode;
		Loader::LoadText("../../Data/Shaders/Saturn/vert.spv", shaderCode);

		VkShaderModuleCreateInfo shaderModuleCreateInfo;
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		shaderModuleCreateInfo.codeSize = shaderCode.size();
		shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();

		VkShaderModule shaderModule;
		VulkanCommon::CreateShaderModule(device.device, &shaderModuleCreateInfo, &shaderModule);

		shader.shaderModules["Vert"] = shaderModule;
	}

	/// Frag
	{
		std::vector<char> shaderCode;
		Loader::LoadText("../../Data/Shaders/Saturn/frag.spv", shaderCode);

		VkShaderModuleCreateInfo shaderModuleCreateInfo;
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		shaderModuleCreateInfo.codeSize = shaderCode.size();
		shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();

		VkShaderModule shaderModule;
		VulkanCommon::CreateShaderModule(device.device, &shaderModuleCreateInfo, &shaderModule);

		shader.shaderModules["Frag"] = shaderModule;
	}

	/// Comp
	{
		std::vector<char> shaderCode;
		Loader::LoadText("../../Data/Shaders/Saturn/comp.spv", shaderCode);

		VkShaderModuleCreateInfo shaderModuleCreateInfo;
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.pNext = nullptr;
		shaderModuleCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		shaderModuleCreateInfo.codeSize = shaderCode.size();
		shaderModuleCreateInfo.pCode = (uint32_t*)shaderCode.data();

		VkShaderModule shaderModule;
		VulkanCommon::CreateShaderModule(device.device, &shaderModuleCreateInfo, &shaderModule);

		shader.shaderModules["Comp"] = shaderModule;
	}
}
void Renderer::InitPipeline()
{
	/// Layout
	// Vertex1Uniform_Fragment1Sampler
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
			descriptor.descriptorSetLayouts["Vertex_1Uniform"],
			descriptor.descriptorSetLayouts["Fragment_1Sampler"],
		};
		std::vector<VkPushConstantRange> pushConstantRanges = {
			//VulkanCommon::GetVkPushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(float) * 4 + sizeof(uint32_t)),
		};

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = (uint32_t)pushConstantRanges.size();
		pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		VkPipelineLayout pipelineLayout;
		VulkanCommon::CreatePipelineLayout(device.device, &pipelineLayoutCreateInfo, &pipelineLayout);

		pipeline.pipelineLayouts["Vertex1Uniform_Fragment1Sampler"] = pipelineLayout;
	}

	// Compute_1Uniform2Storage
	{
		std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {
			descriptor.descriptorSetLayouts["Compute_1Uniform2Storage"],
		};
		std::vector<VkPushConstantRange> pushConstantRanges = { };

		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo;
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.pNext = nullptr;
		pipelineLayoutCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineLayoutCreateInfo.setLayoutCount = (uint32_t)descriptorSetLayouts.size();
		pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
		pipelineLayoutCreateInfo.pushConstantRangeCount = (uint32_t)pushConstantRanges.size();
		pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();

		VkPipelineLayout pipelineLayout;
		VulkanCommon::CreatePipelineLayout(device.device, &pipelineLayoutCreateInfo, &pipelineLayout);

		pipeline.pipelineLayouts["Compute_1Uniform2Storage"] = pipelineLayout;
	}

	/// Pipeline Info
	// Shader
	pipeline.shaderStages["Basic"] = {
		VulkanCommon::GetVkPipelineShaderStageCreateInfo(VK_SHADER_STAGE_VERTEX_BIT, shader.shaderModules["Vert"]),
		VulkanCommon::GetVkPipelineShaderStageCreateInfo(VK_SHADER_STAGE_FRAGMENT_BIT, shader.shaderModules["Frag"]),
	};
	pipeline.shaderStages["Compute"] = {
		VulkanCommon::GetVkPipelineShaderStageCreateInfo(VK_SHADER_STAGE_COMPUTE_BIT, shader.shaderModules["Comp"]),
	};

	// Vertex Input
	pipeline.inputBindings["IPos3Col3Rot3ScaleLightCount_VPos3_VUvNormal"] = VulkanCommon::GetVertexInputBindingDescriptions({
		VulkanCommon::BindingDescriptionInfo::Get(Structure::Get(0, memory.instanceStruct.GetDatas()), VK_VERTEX_INPUT_RATE_INSTANCE),
		VulkanCommon::BindingDescriptionInfo::Get(Structure::Get(0,{
			Structure::Data::Get("Position3", Structure::DATATYPE::FLOAT, 3),
		}), VK_VERTEX_INPUT_RATE_VERTEX),
		VulkanCommon::BindingDescriptionInfo::Get(Structure::Get(0,{
			Structure::Data::Get("Uv", Structure::DATATYPE::FLOAT, 2),
			Structure::Data::Get("Normal", Structure::DATATYPE::FLOAT, 3),
		}), VK_VERTEX_INPUT_RATE_VERTEX),
	});
	pipeline.inputAttributes["Pos3Col3Rot3ScaleLightCount_Pos3_UvNormal"] = { VulkanCommon::GetVertexInputAttributeDescriptions({
		Structure::Get(0, memory.instanceStruct.GetDatas()),
		Structure::Get(0,{
			Structure::Data::Get("Position3", Structure::DATATYPE::FLOAT, 3),
		}),
		Structure::Get(0,{
			Structure::Data::Get("Uv", Structure::DATATYPE::FLOAT, 2),
			Structure::Data::Get("Normal", Structure::DATATYPE::FLOAT, 3),
		}),
	})};
	pipeline.vertexInputStates["Basic"] = VulkanCommon::GetVkPipelineVertexInputStateCreateInfo(&pipeline.inputBindings["IPos3Col3Rot3ScaleLightCount_VPos3_VUvNormal"], &pipeline.inputAttributes["Pos3Col3Rot3ScaleLightCount_Pos3_UvNormal"]);

	// Input Assembly
	pipeline.inputAssemblyStates["Basic"] = VulkanCommon::GetVkPipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, VK_FALSE);

	// Tessalation
	pipeline.tessalationStates["Basic"] = VulkanCommon::GetVkPipelineTessellationStateCreateInfo(0);

	// Viewport
	pipeline.viewports["ClientSpace"] = presentation.swapchainMap.begin()->second.viewports;
	pipeline.scissors["ClientSpace"] = presentation.swapchainMap.begin()->second.scissors;
	pipeline.viewportStates["Basic"] = VulkanCommon::GetVkPipelineViewportStateCreateInfo(pipeline.viewports["ClientSpace"], pipeline.scissors["ClientSpace"]);

	// Rasterization
	pipeline.rasterizationStates["Basic"] = VulkanCommon::GetVkPipelineRasterizationStateCreateInfo(VK_FALSE, VK_FALSE, VK_POLYGON_MODE_FILL, VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);

	// Multisample
	pipeline.multisampleStates["Basic"] = VulkanCommon::GetVkPipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE);

	// Depth Stencil
	pipeline.depthStencilStates["Basic"] = VulkanCommon::GetVkPipelineDepthStencilStateCreateInfo(VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS, VK_FALSE, VK_FALSE, {}, {}, 0.0f, 1.0f);

	// Color Blend
	pipeline.colorBlendAttachment["AdditiveBlend"] = { 
		VulkanCommon::GetVkPipelineColorBlendAttachmentState(VK_FALSE, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_BLEND_FACTOR_ZERO, VK_BLEND_FACTOR_ZERO, VK_BLEND_OP_ADD, VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT),
	};
	pipeline.colorBlendStates["Basic"] = VulkanCommon::GetVkPipelineColorBlendStateCreateInfo(VK_FALSE, VK_LOGIC_OP_COPY, &pipeline.colorBlendAttachment["AdditiveBlend"], 0.0f, 0.0f, 0.0f, 0.0f);

	// Dynamic
	pipeline.dynamic["Viewport_Scissor"] = {
		{ 
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR,
		},
	};
	pipeline.dynamicStates["Basic"] = VulkanCommon::GetVkPipelineDynamicStateCreateInfo(&pipeline.dynamic["Viewport_Scissor"]);

	/// Graphics Pipeline
	pipeline.graphicsPipelineIndices["Basic"] = 0;
	std::vector<VkPipelineShaderStageCreateInfo>* shaderStageState = &pipeline.shaderStages["Basic"];
	pipeline.graphicsPipelineInfos.push_back(VulkanCommon::GetVkGraphicsPipelineCreateInfo(
		(uint32_t)shaderStageState->size(),
		shaderStageState->data(),
		&pipeline.vertexInputStates		["Basic"],
		&pipeline.inputAssemblyStates	["Basic"],
		&pipeline.tessalationStates		["Basic"],
		&pipeline.viewportStates		["Basic"],
		&pipeline.rasterizationStates	["Basic"],
		&pipeline.multisampleStates		["Basic"],
		&pipeline.depthStencilStates	["Basic"],
		&pipeline.colorBlendStates		["Basic"],
		&pipeline.dynamicStates			["Basic"],
		pipeline.pipelineLayouts		["Vertex1Uniform_Fragment1Sampler"],
		presentation.swapchainMap.begin()->second.renderPass,
		VK_NULL_HANDLE
	));

	pipeline.graphicsPipelines.resize(pipeline.graphicsPipelineInfos.size());
	VulkanCommon::CreateGraphicsPipelines(device.device, (uint32_t)pipeline.graphicsPipelineInfos.size(), pipeline.graphicsPipelineInfos.data(), pipeline.graphicsPipelines.data());

	/// Compute Pipeline
	pipeline.computePipelineIndices["Basic"] = 0;
	pipeline.computePipelineInfos.push_back(VulkanCommon::GetVkComputePipelineCreateInfo(
		pipeline.shaderStages["Compute"][0],
		pipeline.pipelineLayouts["Compute_1Uniform2Storage"],
		VK_NULL_HANDLE
	));

	pipeline.computePipelines.resize(pipeline.computePipelineInfos.size());
	VulkanCommon::CreateComputePipelines(device.device, (uint32_t)pipeline.computePipelineInfos.size(), pipeline.computePipelineInfos.data(), pipeline.computePipelines.data());
}

void Renderer::WriteComputeCommandBuffers()
{
	VkCommandBufferInheritanceInfo commandBufferInheritanceInfo;
	commandBufferInheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	commandBufferInheritanceInfo.pNext = nullptr;
	commandBufferInheritanceInfo.renderPass = VK_IGNORED;
	commandBufferInheritanceInfo.subpass = VK_IGNORED;
	commandBufferInheritanceInfo.framebuffer = VK_IGNORED;
	commandBufferInheritanceInfo.occlusionQueryEnable = VK_FALSE;
	commandBufferInheritanceInfo.queryFlags = 0;
	commandBufferInheritanceInfo.pipelineStatistics = 0;

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	commandBufferBeginInfo.pInheritanceInfo = &commandBufferInheritanceInfo;
	VulkanCommon::BeginCommandBuffer(compute.commandBuffer, &commandBufferBeginInfo);

	// Bind compute pipeline
	vkCmdBindPipeline(compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline.computePipelines[pipeline.computePipelineIndices["Basic"]]);

	// Bind instance
	VkPipelineLayout computePipelineLayout = pipeline.pipelineLayouts["Compute_1Uniform2Storage"];
	VkDescriptorSet computeSet = { descriptor.descriptorSets["Instance"] };
	vkCmdBindDescriptorSets(compute.commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, computePipelineLayout, 0, 1, &computeSet, 0, 0);

	// Compute dispatch
	vkCmdDispatch(compute.commandBuffer,
		((Memory::InstanceUserdata*)(memory.computeVertexBuffer.memoryMap.GetPool("Pool")->GetBuffer("Instance")->userdata))->instanceCount - 2,
		1, 1);

	VulkanCommon::EndCommandBuffer(compute.commandBuffer);
}

void Renderer::UpdateBuffers(float _deltaTime, float _speedMultiplier, Math3D::Mat4 _viewProjection[2])
{
	// DeltaTime
	MemoryMap::BufferNode* deltaTimeBuffer = memory.uniformBuffer.memoryMap.GetPool("Pool")->GetBuffer("DeltaTime");

	float deltaTime = _deltaTime * _speedMultiplier;
	//if (deltaTime > 8.0f)
	//	deltaTime = 8.0f;

	memory.transferManager.CpuTransfer(&deltaTime, sizeof(deltaTime), memory.uniformBuffer.handle, deltaTimeBuffer->memoryInfo.offset);
	deltaTimeBuffer->state = MemoryMap::MEMORY_STATE::VALID;

	// ViewProjection
	MemoryMap::BufferNode* viewProjectionBuffer = memory.uniformBuffer.memoryMap.GetPool("Pool")->GetBuffer("ViewProjection");

	memory.transferManager.CpuTransfer(_viewProjection, sizeof(Math3D::Mat4) * 2, memory.uniformBuffer.handle, viewProjectionBuffer->memoryInfo.offset);
	viewProjectionBuffer->state = MemoryMap::MEMORY_STATE::VALID;
}

void Renderer::WriteGraphicsCommandBuffers()
{
	// Begin recording graphics
	Swapchain* swapchain = &presentation.swapchainMap["Window"];
	swapchain->PrepareToRecord(device.device);
	VkCommandBuffer commandBuffer = swapchain->BeginRecording();

	// Bind graphics pipeline
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.graphicsPipelines[pipeline.graphicsPipelineIndices["Basic"]]);

	// Bind View Projection
	VkPipelineLayout graphicsPipelineLayout = pipeline.pipelineLayouts["Vertex1Uniform_Fragment1Sampler"];
	VkDescriptorSet set0 = descriptor.descriptorSets["ViewProjection"];
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 0, 1, &set0, 0, nullptr);

	// Bind vertices
	MemoryMap::PoolNode* vertexPool = memory.vertexBuffer.memoryMap.GetPool("Pool");
	MemoryMap::PoolNode* indexPool = memory.indexBuffer.memoryMap.GetPool("Pool");
	MemoryMap::PoolNode* computeVertexPool = memory.computeVertexBuffer.memoryMap.GetPool("Pool");
	VkBuffer vBuffers[] = { memory.computeVertexBuffer.handle, memory.vertexBuffer.handle, memory.vertexBuffer.handle };

	MemoryMap::BufferNode* Instance = computeVertexPool->GetBuffer("Instance");

	// Asteroid
	MemoryMap::BufferNode* IcoPos3 = vertexPool->GetBuffer("IcoPos3");
	MemoryMap::BufferNode* IcoUvNormal = vertexPool->GetBuffer("IcoUvNormal");

	VkDeviceSize vOffsetsIco[] = { Instance->memoryInfo.offset, IcoPos3->memoryInfo.offset, IcoUvNormal->memoryInfo.offset };
	vkCmdBindVertexBuffers(commandBuffer, 0, sizeof(vBuffers) / sizeof(VkBuffer), vBuffers, vOffsetsIco);

	// Bind indices
	MemoryMap::BufferNode* IcoIndex = indexPool->GetBuffer("IcoIndex");
	vkCmdBindIndexBuffer(commandBuffer, memory.indexBuffer.handle, IcoIndex->memoryInfo.offset, ((Memory::IndexUserdata*)IcoIndex->userdata)->indexType);

	// Bind textures
	VkDescriptorSet set1 = descriptor.descriptorSets["Asteroid"];
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 1, 1, &set1, 0, nullptr);

	// Draw call
	vkCmdDrawIndexed(commandBuffer, ((Memory::IndexUserdata*)IcoIndex->userdata)->indexCount, ((Memory::InstanceUserdata*)Instance->userdata)->instanceCount-2, 0, 0, 2);

	// Saturn
	MemoryMap::BufferNode* SaturnPos3 = vertexPool->GetBuffer("SaturnPos3");
	MemoryMap::BufferNode* SaturnUvNormal = vertexPool->GetBuffer("SaturnUvNormal");

	VkDeviceSize vOffsetsSaturn[] = { Instance->memoryInfo.offset, SaturnPos3->memoryInfo.offset, SaturnUvNormal->memoryInfo.offset };
	vkCmdBindVertexBuffers(commandBuffer, 0, sizeof(vBuffers) / sizeof(VkBuffer), vBuffers, vOffsetsSaturn);

	MemoryMap::BufferNode* SaturnIndex = indexPool->GetBuffer("SaturnIndex");
	vkCmdBindIndexBuffer(commandBuffer, memory.indexBuffer.handle, SaturnIndex->memoryInfo.offset, ((Memory::IndexUserdata*)SaturnIndex->userdata)->indexType);

	set1 = descriptor.descriptorSets["Saturn"];
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 1, 1, &set1, 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, ((Memory::IndexUserdata*)SaturnIndex->userdata)->indexCount, 1, 0, 0, 0);

	// CubeMap
	MemoryMap::BufferNode* CubeMapPos3 = vertexPool->GetBuffer("CubeMapPos3");
	MemoryMap::BufferNode* CubeMapUvNormal = vertexPool->GetBuffer("CubeMapUvNormal");

	VkDeviceSize vOffsetsCubeMap[] = { Instance->memoryInfo.offset, CubeMapPos3->memoryInfo.offset, CubeMapUvNormal->memoryInfo.offset };
	vkCmdBindVertexBuffers(commandBuffer, 0, sizeof(vBuffers) / sizeof(VkBuffer), vBuffers, vOffsetsCubeMap);

	MemoryMap::BufferNode* CubeMapIndex = indexPool->GetBuffer("CubeMapIndex");
	vkCmdBindIndexBuffer(commandBuffer, memory.indexBuffer.handle, CubeMapIndex->memoryInfo.offset, ((Memory::IndexUserdata*)CubeMapIndex->userdata)->indexType);

	set1 = descriptor.descriptorSets["CubeMap"];
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipelineLayout, 1, 1, &set1, 0, nullptr);

	vkCmdDrawIndexed(commandBuffer, ((Memory::IndexUserdata*)CubeMapIndex->userdata)->indexCount, 1, 0, 0, 1);

	// End recording
	swapchain->EndRecording();
}

void Renderer::Present()
{
	VulkanCommon::WaitForFences(device.device, 1, &presentation.renderDone, VK_TRUE, ~0U);
	VulkanCommon::ResetFences(device.device, 1, &presentation.renderDone);

	Swapchain* swapchain = &presentation.swapchainMap["Window"];

	VkCommandBufferBeginInfo commandBufferBeginInfo;
	commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	commandBufferBeginInfo.pNext = nullptr;
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	commandBufferBeginInfo.pInheritanceInfo = nullptr;
	VulkanCommon::BeginCommandBuffer(presentation.submitCommandBuffer, &commandBufferBeginInfo);

	vkCmdExecuteCommands(presentation.submitCommandBuffer, 1, &compute.commandBuffer);

	VkClearValue clearColor[2];
	clearColor[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
	clearColor[1].depthStencil = { 1.0f, 0 };

	VkRenderPassBeginInfo renderPassBeginInfo;
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.pNext = nullptr;
	renderPassBeginInfo.renderPass = swapchain->renderPass;
	renderPassBeginInfo.framebuffer = swapchain->framebuffers[swapchain->imageIndex];
	renderPassBeginInfo.renderArea.offset = { 0, 0 };
	renderPassBeginInfo.renderArea.extent = swapchain->extent;
	renderPassBeginInfo.clearValueCount = sizeof(clearColor) / sizeof(VkClearValue);
	renderPassBeginInfo.pClearValues = clearColor;

	vkCmdBeginRenderPass(presentation.submitCommandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
	vkCmdExecuteCommands(presentation.submitCommandBuffer, 1, &swapchain->commandBuffers[swapchain->imageIndex]);
	vkCmdEndRenderPass(presentation.submitCommandBuffer);

	VulkanCommon::EndCommandBuffer(presentation.submitCommandBuffer);

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	std::vector<VkSemaphore> waitSemaphores = { swapchain->imageAvailableSemaphore };

	VkSubmitInfo submitInfo;
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.waitSemaphoreCount = (uint32_t)waitSemaphores.size();
	submitInfo.pWaitSemaphores = waitSemaphores.data();
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &presentation.submitCommandBuffer;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &swapchain->renderDoneSemaphore;
	VulkanCommon::QueueSubmit(presentation.renderQueue, 1, &submitInfo, presentation.renderDone);

	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &swapchain->renderDoneSemaphore;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain->handle;
	presentInfo.pImageIndices = &swapchain->imageIndex;
	presentInfo.pResults = nullptr;
	VulkanCommon::QueuePresentKHR(presentation.renderQueue, &presentInfo);
}

void Renderer::ShutDown()
{
	memory.vertexBuffer.ShutDown();
	memory.computeVertexBuffer.ShutDown();
	memory.computeBuffer.ShutDown();
	memory.indexBuffer.ShutDown();
	memory.uniformBuffer.ShutDown();
	memory.RGBA8Images.ShutDown();
	memory.depthImages.ShutDown();

	memory.transferManager.ShutDown();
}
