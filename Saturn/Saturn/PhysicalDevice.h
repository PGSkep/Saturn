#ifndef	PHYSICAL_DEVICE_H
#define PHYSICAL_DEVICE_H

#include <vector>

#include "VulkanCommon.h"

class PhysicalDevice
{
public:
	struct Presentability
	{
		VkBool32 win32Presentability = VK_FALSE;
		std::vector<VkBool32> perSurfacePresentability;
	};
	struct SurfaceProperties
	{
		VkSurfaceCapabilitiesKHR surfaceCapabilities;
		std::vector<VkSurfaceFormatKHR> surfaceFormats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	VkPhysicalDevice						handle = VK_NULL_HANDLE;
	VkPhysicalDeviceFeatures				features;
	VkPhysicalDeviceMemoryProperties		memoryProperties;
	VkPhysicalDeviceProperties				properties;
	std::vector<VkQueueFamilyProperties>	queueFamilyProperties;
	std::vector<Presentability>				queueFamilyPresentability;
	std::vector<VkLayerProperties>			layers;
	std::vector<VkExtensionProperties>		extensions;
	VkFormat								depthFormat;

	std::vector<SurfaceProperties>			surfaceProperties;

	void Init(VkPhysicalDevice _physicalDevice, std::vector<VkSurfaceKHR> _surfaces, std::vector<VkFormat> _depthFormats)
	{
		handle = _physicalDevice;

		vkGetPhysicalDeviceFeatures(handle, &features);
		vkGetPhysicalDeviceMemoryProperties(handle, &memoryProperties);
		vkGetPhysicalDeviceProperties(handle, &properties);

		uint32_t propertyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &propertyCount, nullptr);
		queueFamilyProperties.resize(propertyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(handle, &propertyCount, queueFamilyProperties.data());

		queueFamilyPresentability.resize(queueFamilyProperties.size());
		for (uint32_t iQueueFamily = 0; iQueueFamily != queueFamilyPresentability.size(); ++iQueueFamily)
		{
			queueFamilyPresentability[iQueueFamily].win32Presentability = vkGetPhysicalDeviceWin32PresentationSupportKHR(handle, iQueueFamily);

			queueFamilyPresentability[iQueueFamily].perSurfacePresentability.resize(_surfaces.size());

			uint32_t iSurface = 0;
			for (size_t iSurface = 0; iSurface != _surfaces.size(); ++iSurface)
			{
				VkBool32* surfacePresentablePtr = &queueFamilyPresentability[iQueueFamily].perSurfacePresentability[iSurface];
				VulkanCommon::GetPhysicalDeviceSurfaceSupportKHR(handle, iQueueFamily, _surfaces[iSurface], surfacePresentablePtr);
			}
		}

		VulkanCommon::EnumerateDeviceLayerProperties(handle, layers);
		VulkanCommon::EnumerateDeviceExtensionProperties(handle, extensions);

		for (uint32_t iFormat = 0; iFormat != _depthFormats.size(); ++iFormat)
		{
			VkFormatProperties formatProperties;
			vkGetPhysicalDeviceFormatProperties(handle, _depthFormats[iFormat], &formatProperties);

			if ((formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) == VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				depthFormat = _depthFormats[iFormat];
				break;
			}
		}

		surfaceProperties.resize(_surfaces.size());
		for (size_t iSurface = 0; iSurface != _surfaces.size(); ++iSurface)
		{
			VulkanCommon::GetPhysicalDeviceSurfaceCapabilitiesKHR(handle, _surfaces[iSurface], &surfaceProperties[iSurface].surfaceCapabilities);
			VulkanCommon::GetPhysicalDeviceSurfaceFormatsKHR(handle, _surfaces[iSurface], surfaceProperties[iSurface].surfaceFormats);
			VulkanCommon::GetPhysicalDeviceSurfacePresentModesKHR(handle, _surfaces[iSurface], surfaceProperties[iSurface].presentModes);
		}
	}
};

#endif