#ifndef	VKU_H
#define VKU_H

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.h>
#define VK_RESERVED_FOR_FUTURE_USE 0
#define VK_FLAGS_NONE 0

#include <string>
#include <vector>

#include "../../Common/Utility/Error.h"
#include "../../Common/Utility/Console.h"
#include "../../Common/Utility/Vertex.h"
#include "../../Common/Utility/Structure.h"

namespace VkU
{
	uint32_t FindMemoryType(VkPhysicalDeviceMemoryProperties _physicalDeviceMemoryProperties, uint32_t _memoryTypeBits, VkMemoryPropertyFlags _memoryPropertyFlags)
	{
		for (uint32_t i = 0; i < _physicalDeviceMemoryProperties.memoryTypeCount; i++)
			if ((_memoryTypeBits & (1 << i)) && (_physicalDeviceMemoryProperties.memoryTypes[i].propertyFlags & _memoryPropertyFlags) == _memoryPropertyFlags)
				return i;

		return -1;
	}
	static inline VkAccessFlags GetSrcAccessMask(VkImageLayout _oldLayout, VkImageLayout _newLayout)
	{
		if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			return 0;
		else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			return VK_ACCESS_TRANSFER_WRITE_BIT;
		else
			Error::Report("Missing oldLayout/newLayout combination at VkU::GetSrcAccessMask()");

		return -1;
	}
	static inline VkAccessFlags GetDstAccessMask(VkImageLayout _oldLayout, VkImageLayout _newLayout)
	{
		if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			return VK_ACCESS_TRANSFER_WRITE_BIT;
		else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			return VK_ACCESS_SHADER_READ_BIT;
		else
			Error::Report("Missing oldLayout/newLayout combination at VkU::GetDstAccessMask()");

		return -1;
	}

	static inline VkPipelineStageFlags GetSrcStageMask(VkImageLayout _oldLayout, VkImageLayout _newLayout)
	{
		if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			return VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			return VK_PIPELINE_STAGE_TRANSFER_BIT;
		else
			Error::Report("Missing oldLayout/newLayout combination at VkU::GetSrcStageMask()");

		return -1;
	}
	static inline VkPipelineStageFlags GetDstStageMask(VkImageLayout _oldLayout, VkImageLayout _newLayout)
	{
		if (_oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && _newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
			return VK_PIPELINE_STAGE_TRANSFER_BIT;
		else if (_oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && _newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			return VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		else
			Error::Report("Missing oldLayout/newLayout combination at VkU::GetDstStageMask()");

		return -1;
	}

	// Vulkan.h struct dependent
	std::string GetStrFromVkResult(VkResult _result)
	{
		switch (_result)
		{
		case VK_SUCCESS:							return "VK_SUCCESS";
		case VK_NOT_READY:							return "VK_NOT_READY";
		case VK_TIMEOUT:							return "VK_TIMEOUT";
		case VK_EVENT_SET:							return "VK_EVENT_SET";
		case VK_EVENT_RESET:						return "VK_EVENT_RESET";
		case VK_INCOMPLETE:							return "VK_INCOMPLETE";
		case VK_ERROR_OUT_OF_HOST_MEMORY:			return "VK_ERROR_OUT_OF_HOST_MEMORY";
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:			return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
		case VK_ERROR_INITIALIZATION_FAILED:		return "VK_ERROR_INITIALIZATION_FAILED";
		case VK_ERROR_DEVICE_LOST:					return "VK_ERROR_DEVICE_LOST";
		case VK_ERROR_MEMORY_MAP_FAILED:			return "VK_ERROR_MEMORY_MAP_FAILED";
		case VK_ERROR_LAYER_NOT_PRESENT:			return "VK_ERROR_LAYER_NOT_PRESENT";
		case VK_ERROR_EXTENSION_NOT_PRESENT:		return "VK_ERROR_EXTENSION_NOT_PRESENT";
		case VK_ERROR_FEATURE_NOT_PRESENT:			return "VK_ERROR_FEATURE_NOT_PRESENT";
		case VK_ERROR_INCOMPATIBLE_DRIVER:			return "VK_ERROR_INCOMPATIBLE_DRIVER";
		case VK_ERROR_TOO_MANY_OBJECTS:				return "VK_ERROR_TOO_MANY_OBJECTS";
		case VK_ERROR_FORMAT_NOT_SUPPORTED:			return "VK_ERROR_FORMAT_NOT_SUPPORTED";
		case VK_ERROR_FRAGMENTED_POOL:				return "VK_ERROR_FRAGMENTED_POOL";
		case VK_ERROR_SURFACE_LOST_KHR:				return "VK_ERROR_SURFACE_LOST_KHR";
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:		return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
		case VK_SUBOPTIMAL_KHR:						return "VK_SUBOPTIMAL_KHR";
		case VK_ERROR_OUT_OF_DATE_KHR:				return "VK_ERROR_OUT_OF_DATE_KHR";
		case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:		return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
		case VK_ERROR_VALIDATION_FAILED_EXT:		return "VK_ERROR_VALIDATION_FAILED_EXT";
		case VK_ERROR_INVALID_SHADER_NV:			return "VK_ERROR_INVALID_SHADER_NV";
		case VK_ERROR_OUT_OF_POOL_MEMORY_KHR:		return "VK_ERROR_OUT_OF_POOL_MEMORY_KHR";
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:	return "VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR";
		default: return "UNKNOWN";
		}
	}
	inline VkPhysicalDeviceFeatures PhysicalDevivceGetFeaturesFromStrArr(std::vector<const char*> _featureNames)
	{
		VkPhysicalDeviceFeatures physicalDeviceFeatures = {};

		for (size_t i = 0; i != _featureNames.size(); ++i)
		{
			if (strcmp(_featureNames[i], "robustBufferAccess") == 0)
				physicalDeviceFeatures.robustBufferAccess = VK_TRUE;
			if (strcmp(_featureNames[i], "fullDrawIndexUint32") == 0)
				physicalDeviceFeatures.fullDrawIndexUint32 = VK_TRUE;
			if (strcmp(_featureNames[i], "imageCubeArray") == 0)
				physicalDeviceFeatures.imageCubeArray = VK_TRUE;
			if (strcmp(_featureNames[i], "independentBlend") == 0)
				physicalDeviceFeatures.independentBlend = VK_TRUE;
			if (strcmp(_featureNames[i], "geometryShader") == 0)
				physicalDeviceFeatures.geometryShader = VK_TRUE;
			if (strcmp(_featureNames[i], "tessellationShader") == 0)
				physicalDeviceFeatures.tessellationShader = VK_TRUE;
			if (strcmp(_featureNames[i], "sampleRateShading") == 0)
				physicalDeviceFeatures.sampleRateShading = VK_TRUE;
			if (strcmp(_featureNames[i], "dualSrcBlend") == 0)
				physicalDeviceFeatures.dualSrcBlend = VK_TRUE;
			if (strcmp(_featureNames[i], "logicOp") == 0)
				physicalDeviceFeatures.logicOp = VK_TRUE;
			if (strcmp(_featureNames[i], "multiDrawIndirect") == 0)
				physicalDeviceFeatures.multiDrawIndirect = VK_TRUE;
			if (strcmp(_featureNames[i], "drawIndirectFirstInstance") == 0)
				physicalDeviceFeatures.drawIndirectFirstInstance = VK_TRUE;
			if (strcmp(_featureNames[i], "depthClamp") == 0)
				physicalDeviceFeatures.depthClamp = VK_TRUE;
			if (strcmp(_featureNames[i], "depthBiasClamp") == 0)
				physicalDeviceFeatures.depthBiasClamp = VK_TRUE;
			if (strcmp(_featureNames[i], "fillModeNonSolid") == 0)
				physicalDeviceFeatures.fillModeNonSolid = VK_TRUE;
			if (strcmp(_featureNames[i], "depthBounds") == 0)
				physicalDeviceFeatures.depthBounds = VK_TRUE;
			if (strcmp(_featureNames[i], "wideLines") == 0)
				physicalDeviceFeatures.wideLines = VK_TRUE;
			if (strcmp(_featureNames[i], "largePoints") == 0)
				physicalDeviceFeatures.largePoints = VK_TRUE;
			if (strcmp(_featureNames[i], "alphaToOne") == 0)
				physicalDeviceFeatures.alphaToOne = VK_TRUE;
			if (strcmp(_featureNames[i], "multiViewport") == 0)
				physicalDeviceFeatures.multiViewport = VK_TRUE;
			if (strcmp(_featureNames[i], "samplerAnisotropy") == 0)
				physicalDeviceFeatures.samplerAnisotropy = VK_TRUE;
			if (strcmp(_featureNames[i], "textureCompressionETC2") == 0)
				physicalDeviceFeatures.textureCompressionETC2 = VK_TRUE;
			if (strcmp(_featureNames[i], "textureCompressionASTC_LDR") == 0)
				physicalDeviceFeatures.textureCompressionASTC_LDR = VK_TRUE;
			if (strcmp(_featureNames[i], "textureCompressionBC") == 0)
				physicalDeviceFeatures.textureCompressionBC = VK_TRUE;
			if (strcmp(_featureNames[i], "occlusionQueryPrecise") == 0)
				physicalDeviceFeatures.occlusionQueryPrecise = VK_TRUE;
			if (strcmp(_featureNames[i], "pipelineStatisticsQuery") == 0)
				physicalDeviceFeatures.pipelineStatisticsQuery = VK_TRUE;
			if (strcmp(_featureNames[i], "vertexPipelineStoresAndAtomics") == 0)
				physicalDeviceFeatures.vertexPipelineStoresAndAtomics = VK_TRUE;
			if (strcmp(_featureNames[i], "fragmentStoresAndAtomics") == 0)
				physicalDeviceFeatures.fragmentStoresAndAtomics = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderTessellationAndGeometryPointSize") == 0)
				physicalDeviceFeatures.shaderTessellationAndGeometryPointSize = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderImageGatherExtended") == 0)
				physicalDeviceFeatures.shaderImageGatherExtended = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageExtendedFormats") == 0)
				physicalDeviceFeatures.shaderStorageImageExtendedFormats = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageMultisample") == 0)
				physicalDeviceFeatures.shaderStorageImageMultisample = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageReadWithoutFormat") == 0)
				physicalDeviceFeatures.shaderStorageImageReadWithoutFormat = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageWriteWithoutFormat") == 0)
				physicalDeviceFeatures.shaderStorageImageWriteWithoutFormat = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderUniformBufferArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderUniformBufferArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderSampledImageArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderSampledImageArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageBufferArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderStorageBufferArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderStorageImageArrayDynamicIndexing") == 0)
				physicalDeviceFeatures.shaderStorageImageArrayDynamicIndexing = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderClipDistance") == 0)
				physicalDeviceFeatures.shaderClipDistance = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderCullDistance") == 0)
				physicalDeviceFeatures.shaderCullDistance = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderFloat64") == 0)
				physicalDeviceFeatures.shaderFloat64 = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderInt64") == 0)
				physicalDeviceFeatures.shaderInt64 = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderInt16") == 0)
				physicalDeviceFeatures.shaderInt16 = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderResourceResidency") == 0)
				physicalDeviceFeatures.shaderResourceResidency = VK_TRUE;
			if (strcmp(_featureNames[i], "shaderResourceMinLod") == 0)
				physicalDeviceFeatures.shaderResourceMinLod = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseBinding") == 0)
				physicalDeviceFeatures.sparseBinding = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyBuffer") == 0)
				physicalDeviceFeatures.sparseResidencyBuffer = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyImage2D") == 0)
				physicalDeviceFeatures.sparseResidencyImage2D = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyImage3D") == 0)
				physicalDeviceFeatures.sparseResidencyImage3D = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency2Samples") == 0)
				physicalDeviceFeatures.sparseResidency2Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency4Samples") == 0)
				physicalDeviceFeatures.sparseResidency4Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency8Samples") == 0)
				physicalDeviceFeatures.sparseResidency8Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidency16Samples") == 0)
				physicalDeviceFeatures.sparseResidency16Samples = VK_TRUE;
			if (strcmp(_featureNames[i], "sparseResidencyAliased") == 0)
				physicalDeviceFeatures.sparseResidencyAliased = VK_TRUE;
			if (strcmp(_featureNames[i], "variableMultisampleRate") == 0)
				physicalDeviceFeatures.variableMultisampleRate = VK_TRUE;
			if (strcmp(_featureNames[i], "inheritedQueries") == 0)
				physicalDeviceFeatures.inheritedQueries = VK_TRUE;
		}

		return physicalDeviceFeatures;
	}
	inline bool IsDepthFormat(VkFormat _format)
	{
		std::vector<VkFormat> depthFormats = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_X8_D24_UNORM_PACK32, VK_FORMAT_D16_UNORM, VK_FORMAT_D16_UNORM_S8_UINT };

		for (size_t i = 0; i != depthFormats.size(); ++i)
		{
			if (depthFormats[i] == _format)
				return true;
		}

		return false;
	}

	/// DELETE Vertex.h
	// Vertex.h struct dependent
	inline std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions(std::vector<Vertex::VERTEXTYPE::vertextype> _datatypes)
	{
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;
		VkVertexInputAttributeDescription vertexInputAttributeDescription;
		uint32_t location = 0;
		for (uint32_t i = 0; i != (uint32_t)_datatypes.size(); ++i)
		{
			uint32_t offset = 0;
			// POS
			if ((_datatypes[i] & Vertex::VERTEXTYPE::POS3) == Vertex::VERTEXTYPE::POS3)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}

			// OTH
			if ((_datatypes[i] & Vertex::VERTEXTYPE::UV) == Vertex::VERTEXTYPE::UV)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 2;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			if ((_datatypes[i] & Vertex::VERTEXTYPE::NORMAL) == Vertex::VERTEXTYPE::NORMAL)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			if ((_datatypes[i] & Vertex::VERTEXTYPE::TANGENT) == Vertex::VERTEXTYPE::TANGENT)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			if ((_datatypes[i] & Vertex::VERTEXTYPE::BITANGENT) == Vertex::VERTEXTYPE::BITANGENT)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}

			// COL
			if ((_datatypes[i] & Vertex::VERTEXTYPE::COLOR1) == Vertex::VERTEXTYPE::COLOR1)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			else if ((_datatypes[i] & Vertex::VERTEXTYPE::COLOR2) == Vertex::VERTEXTYPE::COLOR2)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			else if ((_datatypes[i] & Vertex::VERTEXTYPE::COLOR3) == Vertex::VERTEXTYPE::COLOR3)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			else if ((_datatypes[i] & Vertex::VERTEXTYPE::COLOR4) == Vertex::VERTEXTYPE::COLOR4)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}

			// BONE
			size_t boneCount = Vertex::VERTEXTYPE::GetBoneCount(_datatypes[i]);
			if (boneCount != 0)
			{
				// bone index
				size_t count = boneCount;
				while (count != 0)
				{
					vertexInputAttributeDescription.location = location;
					vertexInputAttributeDescription.binding = i;

					if (count >= 4)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(float) * 4;
						count -= 4;
					}
					else if (count == 3)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(float) * 3;
						count -= 3;
					}
					else if (count == 2)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(float) * 2;
						count -= 2;
					}
					else if (count == 1)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R32_SFLOAT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(float) * 1;
						count -= 1;
					}

					++location;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}

				// bone weight
				count = boneCount;
				while (count != 0)
				{
					vertexInputAttributeDescription.location = location;
					vertexInputAttributeDescription.binding = i;

					if (count >= 4)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16B16A16_UINT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(uint16_t) * 4;
						count -= 4;
					}
					else if (count == 3)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16B16_UINT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(uint16_t) * 3;
						count -= 3;
					}
					else if (count == 2)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16_UINT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(uint16_t) * 2;
						count -= 2;
					}
					else if (count == 1)
					{
						vertexInputAttributeDescription.format = VK_FORMAT_R16_UINT;
						vertexInputAttributeDescription.offset = offset;

						offset += sizeof(uint16_t) * 1;
						count -= 1;
					}

					++location;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}
			}
			
			// INSTANCE
			if ((_datatypes[i] & Vertex::VERTEXTYPE::ROT3) == Vertex::VERTEXTYPE::ROT3)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			if ((_datatypes[i] & Vertex::VERTEXTYPE::SCALE1) == Vertex::VERTEXTYPE::SCALE1)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R32_SFLOAT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			if ((_datatypes[i] & Vertex::VERTEXTYPE::LIGHTCOUNT) == Vertex::VERTEXTYPE::LIGHTCOUNT)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R16_UINT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
			if ((_datatypes[i] & Vertex::VERTEXTYPE::BONECOUNT) == Vertex::VERTEXTYPE::BONECOUNT)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = i;
				vertexInputAttributeDescription.format = VK_FORMAT_R16_UINT;
				vertexInputAttributeDescription.offset = offset;

				++location;
				offset += sizeof(float) * 3;
				vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
			}
		}

		return vertexInputAttributeDescriptions;
	}
	struct BindingDescriptionInfo
	{
		uint32_t						binding;
		Vertex::VERTEXTYPE::vertextype	dataType;
		VkVertexInputRate				inputRate;

		static inline BindingDescriptionInfo Get(uint32_t _binding, Vertex::VERTEXTYPE::vertextype _dataType, VkVertexInputRate _inputRate)
		{
			BindingDescriptionInfo bindingDescriptionInfo;
			bindingDescriptionInfo.binding = _binding;
			bindingDescriptionInfo.dataType = _dataType;
			bindingDescriptionInfo.inputRate = _inputRate;
			return bindingDescriptionInfo;
		}
	};
	inline std::vector<VkVertexInputBindingDescription> GetVertexInputBindingDescriptions(std::vector<BindingDescriptionInfo> _info)
	{
		std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
		VkVertexInputBindingDescription vertexInputBindingDescription;
		for (uint32_t i = 0; i != (uint32_t)_info.size(); ++i)
		{
			vertexInputBindingDescription.binding = _info[i].binding;
			vertexInputBindingDescription.stride = (uint32_t)Vertex::VERTEXTYPE::GetStride(_info[i].dataType);
			vertexInputBindingDescription.inputRate = _info[i].inputRate;

			vertexInputBindingDescriptions.push_back(vertexInputBindingDescription);
		}

		return vertexInputBindingDescriptions;
	}

	// Structure.h dependent
	inline VkIndexType GetIndexType(Structure::DATATYPE::datatype _datatype)
	{
		switch (_datatype)
		{
		case Structure::DATATYPE::UINT16:
			return VK_INDEX_TYPE_UINT16;
		case Structure::DATATYPE::UINT32:
			return VK_INDEX_TYPE_UINT32;
		default:
			return VK_INDEX_TYPE_MAX_ENUM;
		}
	}

	struct BindingDescriptionInfo2
	{
		Structure			structure;
		VkVertexInputRate	inputRate;

		static inline BindingDescriptionInfo2 Get(Structure _structure, VkVertexInputRate _inputRate)
		{
			BindingDescriptionInfo2 bindingDescriptionInfo;
			bindingDescriptionInfo.structure = _structure;
			bindingDescriptionInfo.inputRate = _inputRate;
			return bindingDescriptionInfo;
		}
	};
	inline std::vector<VkVertexInputBindingDescription> GetVertexInputBindingDescriptions(std::vector<BindingDescriptionInfo2> _info)
	{
		std::vector<VkVertexInputBindingDescription> vertexInputBindingDescriptions;
		VkVertexInputBindingDescription vertexInputBindingDescription;

		for (size_t i = 0; i != _info.size(); ++i)
		{
			vertexInputBindingDescription.binding = (uint32_t)i;
			vertexInputBindingDescription.stride = (uint32_t)_info[i].structure.GetStride();
			vertexInputBindingDescription.inputRate = _info[i].inputRate;

			vertexInputBindingDescriptions.push_back(vertexInputBindingDescription);
		}

		return vertexInputBindingDescriptions;
	}
	inline std::vector<VkVertexInputAttributeDescription> GetVertexInputAttributeDescriptions(std::vector<Structure> _structures)
	{
		std::vector<VkVertexInputAttributeDescription> vertexInputAttributeDescriptions;

		VkVertexInputAttributeDescription vertexInputAttributeDescription;
		uint32_t location = 0;

		for (size_t i = 0; i != _structures.size(); ++i)
		{
			uint32_t offset = 0;

			std::vector<Structure::Data> datas = _structures[i].GetDatas();
			for (size_t j = 0; j != datas.size(); ++j)
			{
				vertexInputAttributeDescription.location = location;
				vertexInputAttributeDescription.binding = (uint32_t)i;

				switch (datas[j].datatype)
				{
				case Structure::DATATYPE::FLOAT:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R32_SFLOAT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32A32_SFLOAT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(float) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;
				case Structure::DATATYPE::DOUBLE:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R64_SFLOAT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64_SFLOAT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64B64_SFLOAT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64B64A64_SFLOAT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(double) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;

				case Structure::DATATYPE::INT8:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R8_SINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R8G8_SINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R8G8B8_SINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R8G8B8A8_SINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(int8_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;
				case Structure::DATATYPE::INT16:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R16_SINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16_SINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16B16_SINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16B16A16_SINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(int16_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;
				case Structure::DATATYPE::INT32:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R32_SINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32_SINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_SINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32A32_SINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(int32_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;
				case Structure::DATATYPE::INT64:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R64_SINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64_SINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64B64_SINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64B64A64_SINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(int64_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;

				case Structure::DATATYPE::UINT8:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R8_UINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R8G8_UINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R8G8B8_UINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R8G8B8A8_UINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(uint8_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;
				case Structure::DATATYPE::UINT16:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R16_UINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16_UINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16B16_UINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R16G16B16A16_UINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(uint16_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;
				case Structure::DATATYPE::UINT32:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R32_UINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32_UINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32_UINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R32G32B32A32_UINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(uint32_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;
				case Structure::DATATYPE::UINT64:
				{
					if (datas[j].count == 1)
						vertexInputAttributeDescription.format = VK_FORMAT_R64_UINT;
					else if (datas[j].count == 2)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64_UINT;
					else if (datas[j].count == 3)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64B64_UINT;
					else if (datas[j].count == 4)
						vertexInputAttributeDescription.format = VK_FORMAT_R64G64B64A64_UINT;
					else
						Error::Report("Count must be between 1 and 4");

					vertexInputAttributeDescription.offset = offset;

					++location;
					offset += sizeof(uint64_t) * (uint32_t)datas[j].count;
					vertexInputAttributeDescriptions.push_back(vertexInputAttributeDescription);
				}break;

				default:
					Error::Report("Unknown Structure::DATATYPE");
				}
			}
		}

		return vertexInputAttributeDescriptions;
	}

	// static
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT _flags, VkDebugReportObjectTypeEXT _objType, uint64_t _obj, size_t _location, int32_t _code, const char* _layerPrefix, const char* _msg, void* _userData)
	{
		if (_flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::RED + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "ERROR:";
		}
		else if (_flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::PINK + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "PERFORMANCE:";
		}
		else if (_flags & VK_DEBUG_REPORT_WARNING_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::YELLOW + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "WARNING:";
		}
		else if (_flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::GREEN + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "	DEBUG:";
		}
		else if (_flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT) {
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::WHITE + Con::Intensity::HIGH) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16)); std::cout << "	INFORMATION:";
		}

		std::cout << _msg << '\n';
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (Con::Color::WHITE + Con::Intensity::LOW) + ((Con::Color::BLACK + Con::Intensity::LOW) * 16));

		return VK_FALSE; // VK_FALSE wont't abort the function that made this call
	}
	static LRESULT WINAPI MainWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
	{
		return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
	}

	// vkFuncf
	void CreateInstance(VkInstanceCreateInfo* _pCreateInfo, VkInstance& _pInstance)
	{
		VkResult vkResult = vkCreateInstance(_pCreateInfo, nullptr, &_pInstance);
		switch (vkResult)
		{
		case VK_SUCCESS:						break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:		Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:		Error::Report("");	return;
		case VK_ERROR_INITIALIZATION_FAILED:	Error::Report("");	return;
		case VK_ERROR_LAYER_NOT_PRESENT:		Error::Report("");	return;
		case VK_ERROR_EXTENSION_NOT_PRESENT:	Error::Report("");	return;
		case VK_ERROR_INCOMPATIBLE_DRIVER:		Error::Report("");	return;
		default:								Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateDebugReportCallbackEXT(VkInstance _instance, const VkDebugReportCallbackCreateInfoEXT* _pCreateInfo, VkDebugReportCallbackEXT& _pCallback)
	{
		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT_FP = (PFN_vkCreateDebugReportCallbackEXT)vkGetInstanceProcAddr(_instance, "vkCreateDebugReportCallbackEXT");
		if (vkCreateDebugReportCallbackEXT_FP == nullptr)
		{
			Error::Report("vkCreateDebugReportCallbackFP == nullptr");	return;
		}
		VkResult vkResult = vkCreateDebugReportCallbackEXT_FP(_instance, _pCreateInfo, nullptr, &_pCallback);
		switch (vkResult)
		{
		case VK_SUCCESS:	break;
		default:			Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateWin32SurfaceKHR(VkInstance _instance, const VkWin32SurfaceCreateInfoKHR* _pCreateInfo, VkSurfaceKHR& _surface)
	{
		VkResult vkResult = vkCreateWin32SurfaceKHR(_instance, _pCreateInfo, nullptr, &_surface);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void EnumeratePhysicalDevices(VkInstance _instance, std::vector<VkPhysicalDevice>& _physicalDevices)
	{
		uint32_t propertyCount = 0;
		VkResult vkResult = vkEnumeratePhysicalDevices(_instance, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}

		_physicalDevices.resize(propertyCount);
		vkResult = vkEnumeratePhysicalDevices(_instance, &propertyCount, _physicalDevices.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void GetPhysicalDeviceSurfaceSupportKHR(VkPhysicalDevice _physicalDevice, uint32_t _queueFamilyIndex, VkSurfaceKHR _surface, VkBool32* _pSupported)
	{
		VkResult vkResult = vkGetPhysicalDeviceSurfaceSupportKHR(_physicalDevice, _queueFamilyIndex, _surface, _pSupported);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void GetPhysicalDeviceSurfacePresentModesKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, std::vector<VkPresentModeKHR>& _pPresentModes)
	{
		uint32_t propertyCount = 0;
		VkResult vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
		_pPresentModes.resize(propertyCount);
		vkResult = vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice, _surface, &propertyCount, _pPresentModes.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void EnumerateDeviceLayerProperties(VkPhysicalDevice _physicalDevice, std::vector<VkLayerProperties>& _layers)
	{
		uint32_t propertyCount = 0;
		VkResult vkResult = vkEnumerateDeviceLayerProperties(_physicalDevice, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					Error::Report("");	break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report(""); return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report(""); return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}

		_layers.resize(propertyCount);
		vkResult = vkEnumerateDeviceLayerProperties(_physicalDevice, &propertyCount, _layers.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					Error::Report("");	break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void EnumerateDeviceExtensionProperties(VkPhysicalDevice _physicalDevice, std::vector<VkExtensionProperties>& _extensions)
	{
		uint32_t propertyCount = 0;
		VkResult vkResult = vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					Error::Report("");	break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_LAYER_NOT_PRESENT:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}

		_extensions.resize(propertyCount);
		vkResult = vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr, &propertyCount, _extensions.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_INCOMPLETE:					Error::Report(""); break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report(""); return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report(""); return;
		case VK_ERROR_LAYER_NOT_PRESENT:	Error::Report(""); return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateDevice(VkPhysicalDevice _physicalDevice, const VkDeviceCreateInfo* _pCreateInfo, VkDevice& _pDevice)
	{
		VkResult vkResult = vkCreateDevice(_physicalDevice, _pCreateInfo, nullptr, &_pDevice);
		switch (vkResult)
		{
		case VK_SUCCESS:						break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:		Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:		Error::Report("");	return;
		case VK_ERROR_INITIALIZATION_FAILED:	Error::Report("");	return;
		case VK_ERROR_EXTENSION_NOT_PRESENT:	Error::Report("");	return;
		case VK_ERROR_FEATURE_NOT_PRESENT:		Error::Report("");	return;
		case VK_ERROR_TOO_MANY_OBJECTS:			Error::Report("");	return;
		default:								Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void GetPhysicalDeviceSurfaceCapabilitiesKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, VkSurfaceCapabilitiesKHR* _pSurfaceCapabilities)
	{
		VkResult vkResult = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physicalDevice, _surface, _pSurfaceCapabilities);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void GetPhysicalDeviceSurfaceFormatsKHR(VkPhysicalDevice _physicalDevice, VkSurfaceKHR _surface, std::vector<VkSurfaceFormatKHR>& _pSurfaceFormats)
	{
		uint32_t propertyCount = 0;
		VkResult vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
		_pSurfaceFormats.resize(propertyCount);
		vkResult = vkGetPhysicalDeviceSurfaceFormatsKHR(_physicalDevice, _surface, &propertyCount, _pSurfaceFormats.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateRenderPass(VkDevice _device, const VkRenderPassCreateInfo* _pCreateInfo, VkRenderPass* _pRenderPass)
	{
		VkResult vkResult = vkCreateRenderPass(_device, _pCreateInfo, nullptr, _pRenderPass);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void GetSwapchainImagesKHR(VkDevice _device, VkSwapchainKHR _swapchain, std::vector<VkImage>& _images)
	{
		uint32_t propertyCount = 0;
		VkResult vkResult = vkGetSwapchainImagesKHR(_device, _swapchain, &propertyCount, nullptr);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
		_images.resize(propertyCount);
		vkResult = vkGetSwapchainImagesKHR(_device, _swapchain, &propertyCount, _images.data());
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_INCOMPLETE:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateCommandPool(VkDevice _device, const VkCommandPoolCreateInfo* _pCreateInfo, VkCommandPool* _pCommandPool)
	{
		VkResult vkResult = vkCreateCommandPool(_device, _pCreateInfo, nullptr, _pCommandPool);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateSwapchain(VkDevice _device, const VkSwapchainCreateInfoKHR* _pCreateInfo, VkSwapchainKHR* _pSwapchain)
	{
		VkResult vkResult = vkCreateSwapchainKHR(_device, _pCreateInfo, nullptr, _pSwapchain);
		switch (vkResult)
		{
		case VK_SUCCESS:						break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:		Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:		Error::Report("");	return;
		case VK_ERROR_DEVICE_LOST:				Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:			Error::Report("");	return;
		case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:	Error::Report("");	return;
		default:								Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateImage(VkDevice _device, const VkImageCreateInfo* _pCreateInfo, VkImage* _pImage)
	{
		VkResult vkResult = vkCreateImage(_device, _pCreateInfo, nullptr, _pImage);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateImageView(VkDevice _device, const VkImageViewCreateInfo* _pCreateInfo, VkImageView* _pView)
	{
		VkResult vkResult = vkCreateImageView(_device, _pCreateInfo, nullptr, _pView);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void AllocateMemory(VkDevice _device, const VkMemoryAllocateInfo* _pAllocateInfo, VkDeviceMemory* _pMemory)
	{
		VkResult vkResult = vkAllocateMemory(_device, _pAllocateInfo, nullptr, _pMemory);
		switch (vkResult)
		{
		case VK_SUCCESS:							break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:			Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:			Error::Report("");	return;
		case VK_ERROR_TOO_MANY_OBJECTS:				Error::Report("");	return;
		case VK_ERROR_INVALID_EXTERNAL_HANDLE_KHR:	Error::Report("");	return;
		default:									Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void BindImageMemory(VkDevice _device, VkImage _image, VkDeviceMemory _memory, VkDeviceSize _memoryOffset)
	{
		VkResult vkResult = vkBindImageMemory(_device, _image, _memory, _memoryOffset);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateFence(VkDevice _device, const VkFenceCreateInfo* _pCreateInfo, VkFence* _pFence)
	{
		VkResult vkResult = vkCreateFence(_device, _pCreateInfo, nullptr, _pFence);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void AllocateCommandBuffers(VkDevice _device, const VkCommandBufferAllocateInfo* _pAllocateInfo, VkCommandBuffer* _pCommandBuffers)
	{
		VkResult vkResult = vkAllocateCommandBuffers(_device, _pAllocateInfo, _pCommandBuffers);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void WaitForFences(VkDevice _device, uint32_t _fenceCount, const VkFence* _pFences, VkBool32 _waitAll, uint64_t _timeout)
	{
		VkResult vkResult = vkWaitForFences(_device, _fenceCount, _pFences, _waitAll, _timeout);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_DEVICE_LOST:			Error::Report("");	return;
		case VK_TIMEOUT:					Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void ResetFences(VkDevice _device, uint32_t _fenceCount, const VkFence* _pFences)
	{
		VkResult vkResult = vkResetFences(_device, _fenceCount, _pFences);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void BeginCommandBuffer(VkCommandBuffer _commandBuffer, const VkCommandBufferBeginInfo* _pBeginInfo)
	{
		VkResult vkResult = vkBeginCommandBuffer(_commandBuffer, _pBeginInfo);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void EndCommandBuffer(VkCommandBuffer _commandBuffer)
	{
		VkResult vkResult = vkEndCommandBuffer(_commandBuffer);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void QueueSubmit(VkQueue _queue, uint32_t _submitCount, const VkSubmitInfo* _pSubmits, VkFence _fence)
	{
		VkResult vkResult = vkQueueSubmit(_queue, _submitCount, _pSubmits, _fence);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_DEVICE_LOST:			Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateSemaphore_(VkDevice _device, const VkSemaphoreCreateInfo* _pCreateInfo, VkSemaphore* _pSemaphore)
	{
		VkResult vkResult = vkCreateSemaphore(_device, _pCreateInfo, nullptr, _pSemaphore);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateFramebuffer(VkDevice _device, const VkFramebufferCreateInfo* _pCreateInfo, VkFramebuffer* _pFramebuffer)
	{
		VkResult vkResult = vkCreateFramebuffer(_device, _pCreateInfo, nullptr, _pFramebuffer);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateDescriptorSetLayout(VkDevice _device, const VkDescriptorSetLayoutCreateInfo* _pCreateInfo, VkDescriptorSetLayout* _pSetLayout)
	{
		VkResult vkResult = vkCreateDescriptorSetLayout(_device, _pCreateInfo, nullptr, _pSetLayout);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreatePipelineLayout(VkDevice _device, const VkPipelineLayoutCreateInfo* _pCreateInfo, VkPipelineLayout* _pPipelineLayout)
	{
		VkResult vkResult = vkCreatePipelineLayout(_device, _pCreateInfo, nullptr, _pPipelineLayout);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateShaderModule(VkDevice _device, const VkShaderModuleCreateInfo* _pCreateInfo, VkShaderModule* _pShaderModule)
	{
		VkResult vkResult = vkCreateShaderModule(_device, _pCreateInfo, nullptr, _pShaderModule);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateGraphicsPipelines(VkDevice _device, uint32_t _createInfoCount, const VkGraphicsPipelineCreateInfo* _pCreateInfos, VkPipeline* _pPipelines)
	{
		VkResult vkResult = vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, _createInfoCount, _pCreateInfos, nullptr, _pPipelines);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void AcquireNextImageKHR(VkDevice _device, VkSwapchainKHR _swapchain, uint64_t _timeout, VkSemaphore _semaphore, VkFence _fence, uint32_t* _pImageIndex)
	{
		VkResult vkResult = vkAcquireNextImageKHR(_device, _swapchain, _timeout, _semaphore, _fence, _pImageIndex);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_DEVICE_LOST:			Error::Report("");	return;
		case VK_ERROR_OUT_OF_DATE_KHR:		Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		case VK_TIMEOUT:					Error::Report("");	break;
		case VK_NOT_READY:					Error::Report("");	break;
		case VK_SUBOPTIMAL_KHR:				Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void QueuePresentKHR(VkQueue _queue, const VkPresentInfoKHR* _pPresentInfo)
	{
		VkResult vkResult = vkQueuePresentKHR(_queue, _pPresentInfo);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_DEVICE_LOST:			Error::Report("");	return;
		case VK_ERROR_OUT_OF_DATE_KHR:		Error::Report("");	return;
		case VK_ERROR_SURFACE_LOST_KHR:		Error::Report("");	return;
		case VK_SUBOPTIMAL_KHR:				Error::Report("");	break;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateBuffer(VkDevice _device, const VkBufferCreateInfo* _pCreateInfo, VkBuffer* _pBuffer)
	{
		VkResult vkResult = vkCreateBuffer(_device, _pCreateInfo, nullptr, _pBuffer);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void BindBufferMemory(VkDevice _device, VkBuffer _buffer, VkDeviceMemory _memory, VkDeviceSize _memoryOffset)
	{
		VkResult vkResult = vkBindBufferMemory(_device, _buffer, _memory, _memoryOffset);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void MapMemory(VkDevice _device, VkDeviceMemory _memory, VkDeviceSize _offset, VkDeviceSize _size, void** _ppData)
	{
		VkResult vkResult = vkMapMemory(_device, _memory, _offset, _size, VK_FLAGS_NONE, _ppData);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_MEMORY_MAP_FAILED:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateDescriptorPool(VkDevice _device, const VkDescriptorPoolCreateInfo* _pCreateInfo, VkDescriptorPool* _pDescriptorPool)
	{
		VkResult vkResult = vkCreateDescriptorPool(_device, _pCreateInfo, nullptr, _pDescriptorPool);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void AllocateDescriptorSets(VkDevice _device, const VkDescriptorSetAllocateInfo* _pAllocateInfo, VkDescriptorSet* _pDescriptorSets)
	{
		VkResult vkResult = vkAllocateDescriptorSets(_device, _pAllocateInfo, _pDescriptorSets);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_FRAGMENTED_POOL:		Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}
	void CreateSampler(VkDevice _device, const VkSamplerCreateInfo* _pCreateInfo, VkSampler* _pSampler)
	{
		VkResult vkResult = vkCreateSampler(_device, _pCreateInfo, nullptr, _pSampler);
		switch (vkResult)
		{
		case VK_SUCCESS:					break;
		case VK_ERROR_OUT_OF_HOST_MEMORY:	Error::Report("");	return;
		case VK_ERROR_OUT_OF_DEVICE_MEMORY:	Error::Report("");	return;
		case VK_ERROR_TOO_MANY_OBJECTS:		Error::Report("");	return;
		default:							Error::Report(GetStrFromVkResult(vkResult).c_str());	break;
		}
	}

	// Cmd
	void CmdPipelineBarrier(VkCommandBuffer _commandBuffer, VkImage _image, VkImageLayout _oldLayout, VkImageLayout _newLayout)
	{
		VkImageMemoryBarrier imageMemoryBarrier;
		imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		imageMemoryBarrier.pNext = nullptr;
		imageMemoryBarrier.srcAccessMask = VkU::GetSrcAccessMask(_oldLayout, _newLayout);
		imageMemoryBarrier.dstAccessMask = VkU::GetDstAccessMask(_oldLayout, _newLayout);
		imageMemoryBarrier.oldLayout = _oldLayout;
		imageMemoryBarrier.newLayout = _newLayout;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.image = _image;
		imageMemoryBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
		imageMemoryBarrier.subresourceRange.levelCount = 1;
		imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
		imageMemoryBarrier.subresourceRange.layerCount = 1;

		vkCmdPipelineBarrier(_commandBuffer, VkU::GetSrcStageMask(_oldLayout, _newLayout), VkU::GetDstStageMask(_oldLayout, _newLayout), 0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}

	// vkStruct
	static inline VkApplicationInfo GetVkApplicationInfo(const char* _pApplicationName, uint32_t _applicationVersion, const char* _pEngineName, uint32_t _engineVersion, uint32_t _apiVersion)
	{
		VkApplicationInfo applicationInfo;
		applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		applicationInfo.pNext = nullptr;
		applicationInfo.pApplicationName = _pApplicationName;
		applicationInfo.applicationVersion = _applicationVersion;
		applicationInfo.pEngineName = _pEngineName;
		applicationInfo.engineVersion = _engineVersion;
		applicationInfo.apiVersion = _apiVersion;
		return applicationInfo;
	}
	static inline VkDescriptorSetLayoutBinding GetVkDescriptorSetLayoutBinding(uint32_t _binding, VkDescriptorType _descriptorType, uint32_t _descriptorCount, VkShaderStageFlags _stageFlags, const VkSampler* _pImmutableSamplers)
	{
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
		descriptorSetLayoutBinding.binding = _binding;
		descriptorSetLayoutBinding.descriptorType = _descriptorType;
		descriptorSetLayoutBinding.descriptorCount = _descriptorCount;
		descriptorSetLayoutBinding.stageFlags = _stageFlags;
		descriptorSetLayoutBinding.pImmutableSamplers = _pImmutableSamplers;
		return descriptorSetLayoutBinding;
	}
	static inline VkPushConstantRange GetVkPushConstantRange(VkShaderStageFlags _stageFlags, uint32_t _offset, uint32_t _size)
	{
		VkPushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = _stageFlags;
		pushConstantRange.offset = _offset;
		pushConstantRange.size = _size;
		return pushConstantRange;
	}
	static inline VkPipelineShaderStageCreateInfo GetVkPipelineShaderStageCreateInfo(VkShaderStageFlagBits _stage, VkShaderModule _module)
	{
		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo;
		pipelineShaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.pNext = nullptr;
		pipelineShaderStageCreateInfo.flags = VK_FLAGS_NONE;
		pipelineShaderStageCreateInfo.stage = _stage;
		pipelineShaderStageCreateInfo.module = _module;
		pipelineShaderStageCreateInfo.pName = "main";
		pipelineShaderStageCreateInfo.pSpecializationInfo = nullptr;
		return pipelineShaderStageCreateInfo;
	}
	static inline VkVertexInputBindingDescription GetVkVertexInputBindingDescription(uint32_t _binding, uint32_t _stride, VkVertexInputRate _inputRate)
	{
		VkVertexInputBindingDescription vertexInputBindingDescription;
		vertexInputBindingDescription.binding = _binding;
		vertexInputBindingDescription.stride = _stride;
		vertexInputBindingDescription.inputRate = _inputRate;
		return vertexInputBindingDescription;
	}
	static inline VkVertexInputAttributeDescription GetVkVertexInputAttributeDescription(uint32_t _location, uint32_t _binding, VkFormat _format, uint32_t _offset)
	{
		VkVertexInputAttributeDescription vertexInputAttributeDescription;
		vertexInputAttributeDescription.location = _location;
		vertexInputAttributeDescription.binding = _binding;
		vertexInputAttributeDescription.format = _format;
		vertexInputAttributeDescription.offset = _offset;
		return vertexInputAttributeDescription;
	}
	static inline VkPipelineVertexInputStateCreateInfo GetVkPipelineVertexInputStateCreateInfo(uint32_t _vertexBindingDescriptionCount, const VkVertexInputBindingDescription* _pVertexBindingDescriptions, uint32_t _vertexAttributeDescriptionCount, const VkVertexInputAttributeDescription* _pVertexAttributeDescriptions)
	{
		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfo.pNext = nullptr;
		pipelineVertexInputStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = _vertexBindingDescriptionCount;
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = _pVertexBindingDescriptions;
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = _vertexAttributeDescriptionCount;
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = _pVertexAttributeDescriptions;
		return pipelineVertexInputStateCreateInfo;
	}
	static inline VkPipelineVertexInputStateCreateInfo GetVkPipelineVertexInputStateCreateInfo(const std::vector<VkVertexInputBindingDescription>* _vertexBindings, const std::vector<VkVertexInputAttributeDescription>* _vertexAttributes)
	{
		VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo;
		pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		pipelineVertexInputStateCreateInfo.pNext = nullptr;
		pipelineVertexInputStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = (uint32_t)_vertexBindings->size();
		pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = _vertexBindings->data();
		pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = (uint32_t)_vertexAttributes->size();
		pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = _vertexAttributes->data();
		return pipelineVertexInputStateCreateInfo;
	}
	static inline VkPipelineInputAssemblyStateCreateInfo GetVkPipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology _topology, VkBool32 _primitiveRestartEnable)
	{
		VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo;
		pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		pipelineInputAssemblyStateCreateInfo.pNext = nullptr;
		pipelineInputAssemblyStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineInputAssemblyStateCreateInfo.topology = _topology;
		pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = _primitiveRestartEnable;
		return pipelineInputAssemblyStateCreateInfo;
	}
	static inline VkPipelineTessellationStateCreateInfo GetVkPipelineTessellationStateCreateInfo(uint32_t _patchControlPoints)
	{
		VkPipelineTessellationStateCreateInfo pipelineTessellationStateCreateInfo;
		pipelineTessellationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		pipelineTessellationStateCreateInfo.pNext = nullptr;
		pipelineTessellationStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineTessellationStateCreateInfo.patchControlPoints = _patchControlPoints;
		return pipelineTessellationStateCreateInfo;
	}
	static inline VkViewport GetVkViewport(float _x, float _y, float _width, float _height, float _minDepth, float _maxDepth)
	{
		VkViewport viewport;
		viewport.x = _x;
		viewport.y = _y;
		viewport.width = _width;
		viewport.height = _height;
		viewport.minDepth = _minDepth;
		viewport.maxDepth = _maxDepth;
		return viewport;
	}
	static inline VkRect2D GetVkRect2D(VkOffset2D _offset, VkExtent2D _extent)
	{
		VkRect2D scissor;
		scissor.offset = _offset;
		scissor.extent = _extent;
		return scissor;
	}
	static inline VkPipelineViewportStateCreateInfo GetVkPipelineViewportStateCreateInfo(uint32_t _viewportCount, const VkViewport* _pViewports, uint32_t _scissorCount, const VkRect2D* _pScissors)
	{
		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.pNext = nullptr;
		pipelineViewportStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineViewportStateCreateInfo.viewportCount = _viewportCount;
		pipelineViewportStateCreateInfo.pViewports = _pViewports;
		pipelineViewportStateCreateInfo.scissorCount = _scissorCount;
		pipelineViewportStateCreateInfo.pScissors = _pScissors;
		return pipelineViewportStateCreateInfo;
	}
	static inline VkPipelineViewportStateCreateInfo GetVkPipelineViewportStateCreateInfo(const std::vector<VkViewport> _viewports, const std::vector<VkRect2D> _scissors)
	{
		VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo;
		pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCreateInfo.pNext = nullptr;
		pipelineViewportStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineViewportStateCreateInfo.viewportCount = (uint32_t)_viewports.size();
		pipelineViewportStateCreateInfo.pViewports = _viewports.data();
		pipelineViewportStateCreateInfo.scissorCount = (uint32_t)_scissors.size();
		pipelineViewportStateCreateInfo.pScissors = _scissors.data();
		return pipelineViewportStateCreateInfo;
	}
	static inline VkPipelineRasterizationStateCreateInfo GetVkPipelineRasterizationStateCreateInfo(VkBool32 _depthClampEnable, VkBool32 _rasterizerDiscardEnable, VkPolygonMode _polygonMode, VkCullModeFlags _cullMode, VkFrontFace _frontFace, VkBool32 _depthBiasEnable, float _depthBiasConstantFactor, float _depthBiasClamp, float _depthBiasSlopeFactor, float _lineWidth)
	{
		VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo;
		pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		pipelineRasterizationStateCreateInfo.pNext = nullptr;
		pipelineRasterizationStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineRasterizationStateCreateInfo.depthClampEnable = _depthClampEnable;
		pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = _rasterizerDiscardEnable;
		pipelineRasterizationStateCreateInfo.polygonMode = _polygonMode;
		pipelineRasterizationStateCreateInfo.cullMode = _cullMode;
		pipelineRasterizationStateCreateInfo.frontFace = _frontFace;
		pipelineRasterizationStateCreateInfo.depthBiasEnable = _depthBiasEnable;
		pipelineRasterizationStateCreateInfo.depthBiasConstantFactor = _depthBiasConstantFactor;
		pipelineRasterizationStateCreateInfo.depthBiasClamp = _depthBiasClamp;
		pipelineRasterizationStateCreateInfo.depthBiasSlopeFactor = _depthBiasSlopeFactor;
		pipelineRasterizationStateCreateInfo.lineWidth = _lineWidth;
		return pipelineRasterizationStateCreateInfo;
	}
	static inline VkPipelineMultisampleStateCreateInfo GetVkPipelineMultisampleStateCreateInfo(VkSampleCountFlagBits _rasterizationSamples, VkBool32 _sampleShadingEnable, float _minSampleShading, const VkSampleMask* _pSampleMask, VkBool32 _alphaToCoverageEnable, VkBool32 _alphaToOneEnable)
	{
		VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo;
		pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		pipelineMultisampleStateCreateInfo.pNext = nullptr;
		pipelineMultisampleStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineMultisampleStateCreateInfo.rasterizationSamples = _rasterizationSamples;
		pipelineMultisampleStateCreateInfo.sampleShadingEnable = _sampleShadingEnable;
		pipelineMultisampleStateCreateInfo.minSampleShading = _minSampleShading;
		pipelineMultisampleStateCreateInfo.pSampleMask = _pSampleMask;
		pipelineMultisampleStateCreateInfo.alphaToCoverageEnable = _alphaToCoverageEnable;
		pipelineMultisampleStateCreateInfo.alphaToOneEnable = _alphaToOneEnable;
		return pipelineMultisampleStateCreateInfo;
	}
	static inline VkPipelineDepthStencilStateCreateInfo GetVkPipelineDepthStencilStateCreateInfo(VkBool32 _depthTestEnable, VkBool32 _depthWriteEnable, VkCompareOp _depthCompareOp, VkBool32 _depthBoundsTestEnable, VkBool32 _stencilTestEnable, VkStencilOpState _front, VkStencilOpState _back, float _minDepthBounds, float _maxDepthBounds)
	{
		VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo;
		pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		pipelineDepthStencilStateCreateInfo.pNext = nullptr;
		pipelineDepthStencilStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineDepthStencilStateCreateInfo.depthTestEnable = _depthTestEnable;
		pipelineDepthStencilStateCreateInfo.depthWriteEnable = _depthWriteEnable;
		pipelineDepthStencilStateCreateInfo.depthCompareOp = _depthCompareOp;
		pipelineDepthStencilStateCreateInfo.depthBoundsTestEnable = _depthBoundsTestEnable;
		pipelineDepthStencilStateCreateInfo.stencilTestEnable = _stencilTestEnable;
		pipelineDepthStencilStateCreateInfo.front = _front;
		pipelineDepthStencilStateCreateInfo.back = _back;
		pipelineDepthStencilStateCreateInfo.minDepthBounds = _minDepthBounds;
		pipelineDepthStencilStateCreateInfo.maxDepthBounds = _maxDepthBounds;
		return pipelineDepthStencilStateCreateInfo;
	}
	static inline VkPipelineColorBlendAttachmentState GetVkPipelineColorBlendAttachmentState(VkBool32 _blendEnable, VkBlendFactor _srcColorBlendFactor, VkBlendFactor _dstColorBlendFactor, VkBlendOp _colorBlendOp, VkBlendFactor _srcAlphaBlendFactor, VkBlendFactor _dstAlphaBlendFactor, VkBlendOp _alphaBlendOp, VkColorComponentFlags _colorWriteMask)
	{
		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState;
		pipelineColorBlendAttachmentState.blendEnable = _blendEnable;
		pipelineColorBlendAttachmentState.srcColorBlendFactor = _srcColorBlendFactor;
		pipelineColorBlendAttachmentState.dstColorBlendFactor = _dstColorBlendFactor;
		pipelineColorBlendAttachmentState.colorBlendOp = _colorBlendOp;
		pipelineColorBlendAttachmentState.srcAlphaBlendFactor = _srcAlphaBlendFactor;
		pipelineColorBlendAttachmentState.dstAlphaBlendFactor = _dstAlphaBlendFactor;
		pipelineColorBlendAttachmentState.alphaBlendOp = _alphaBlendOp;
		pipelineColorBlendAttachmentState.colorWriteMask = _colorWriteMask;
		return pipelineColorBlendAttachmentState;
	}
	static inline VkPipelineColorBlendStateCreateInfo GetVkPipelineColorBlendStateCreateInfo(VkBool32 _logicOpEnable, VkLogicOp _logicOp, uint32_t _attachmentCount, const VkPipelineColorBlendAttachmentState* _pAttachments, float _blendConstants0, float _blendConstants1, float _blendConstants2, float _blendConstants3)
	{
		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.pNext = nullptr;
		pipelineColorBlendStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineColorBlendStateCreateInfo.logicOpEnable = _logicOpEnable;
		pipelineColorBlendStateCreateInfo.logicOp = _logicOp;
		pipelineColorBlendStateCreateInfo.attachmentCount = _attachmentCount;
		pipelineColorBlendStateCreateInfo.pAttachments = _pAttachments;
		pipelineColorBlendStateCreateInfo.blendConstants[0] = _blendConstants0;
		pipelineColorBlendStateCreateInfo.blendConstants[1] = _blendConstants1;
		pipelineColorBlendStateCreateInfo.blendConstants[2] = _blendConstants2;
		pipelineColorBlendStateCreateInfo.blendConstants[3] = _blendConstants3;
		return pipelineColorBlendStateCreateInfo;
	}
	static inline VkPipelineColorBlendStateCreateInfo GetVkPipelineColorBlendStateCreateInfo(VkBool32 _logicOpEnable, VkLogicOp _logicOp, const std::vector<VkPipelineColorBlendAttachmentState>* _colorBlendAttachments, float _blendConstants0, float _blendConstants1, float _blendConstants2, float _blendConstants3)
	{
		VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo;
		pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		pipelineColorBlendStateCreateInfo.pNext = nullptr;
		pipelineColorBlendStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineColorBlendStateCreateInfo.logicOpEnable = _logicOpEnable;
		pipelineColorBlendStateCreateInfo.logicOp = _logicOp;
		pipelineColorBlendStateCreateInfo.attachmentCount = (uint32_t)_colorBlendAttachments->size();
		pipelineColorBlendStateCreateInfo.pAttachments = _colorBlendAttachments->data();
		pipelineColorBlendStateCreateInfo.blendConstants[0] = _blendConstants0;
		pipelineColorBlendStateCreateInfo.blendConstants[1] = _blendConstants1;
		pipelineColorBlendStateCreateInfo.blendConstants[2] = _blendConstants2;
		pipelineColorBlendStateCreateInfo.blendConstants[3] = _blendConstants3;
		return pipelineColorBlendStateCreateInfo;
	}
	static inline VkPipelineDynamicStateCreateInfo GetVkPipelineDynamicStateCreateInfo(uint32_t _dynamicStateCount, const VkDynamicState* _pDynamicStates)
	{
		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.pNext = nullptr;
		pipelineDynamicStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineDynamicStateCreateInfo.dynamicStateCount = _dynamicStateCount;
		pipelineDynamicStateCreateInfo.pDynamicStates = _pDynamicStates;
		return pipelineDynamicStateCreateInfo;
	}
	static inline VkPipelineDynamicStateCreateInfo GetVkPipelineDynamicStateCreateInfo(const std::vector<VkDynamicState>* _dynamicStates)
	{
		VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo;
		pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		pipelineDynamicStateCreateInfo.pNext = nullptr;
		pipelineDynamicStateCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		pipelineDynamicStateCreateInfo.dynamicStateCount = (uint32_t)_dynamicStates->size();
		pipelineDynamicStateCreateInfo.pDynamicStates = _dynamicStates->data();
		return pipelineDynamicStateCreateInfo;
	}
	static inline VkGraphicsPipelineCreateInfo GetVkGraphicsPipelineCreateInfo(uint32_t _stageCount, const VkPipelineShaderStageCreateInfo* _pStages, const VkPipelineVertexInputStateCreateInfo* _pVertexInputState, const VkPipelineInputAssemblyStateCreateInfo* _pInputAssemblyState, const VkPipelineTessellationStateCreateInfo* _pTessellationState, const VkPipelineViewportStateCreateInfo* _pViewportState, const VkPipelineRasterizationStateCreateInfo* _pRasterizationState, const VkPipelineMultisampleStateCreateInfo* _pMultisampleState, const VkPipelineDepthStencilStateCreateInfo* _pDepthStencilState, const VkPipelineColorBlendStateCreateInfo* _pColorBlendState, const VkPipelineDynamicStateCreateInfo* _pDynamicState, VkPipelineLayout _layout, VkRenderPass _renderPass, VkPipeline _basePipelineHandle)
	{
		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.pNext = nullptr;
		graphicsPipelineCreateInfo.flags = VK_FLAGS_NONE;
		graphicsPipelineCreateInfo.stageCount = _stageCount;
		graphicsPipelineCreateInfo.pStages = _pStages;
		graphicsPipelineCreateInfo.pVertexInputState = _pVertexInputState;
		graphicsPipelineCreateInfo.pInputAssemblyState = _pInputAssemblyState;
		graphicsPipelineCreateInfo.pTessellationState = _pTessellationState;
		graphicsPipelineCreateInfo.pViewportState = _pViewportState;
		graphicsPipelineCreateInfo.pRasterizationState = _pRasterizationState;
		graphicsPipelineCreateInfo.pMultisampleState = _pMultisampleState;
		graphicsPipelineCreateInfo.pDepthStencilState = _pDepthStencilState;
		graphicsPipelineCreateInfo.pColorBlendState = _pColorBlendState;
		graphicsPipelineCreateInfo.pDynamicState = _pDynamicState;
		graphicsPipelineCreateInfo.layout = _layout;
		graphicsPipelineCreateInfo.renderPass = _renderPass;
		graphicsPipelineCreateInfo.subpass = 0;
		graphicsPipelineCreateInfo.basePipelineHandle = _basePipelineHandle;
		graphicsPipelineCreateInfo.basePipelineIndex = 0;
		return graphicsPipelineCreateInfo;
	}

	static inline VkImageCreateInfo GetVkImageCreateInfo(uint32_t _width, uint32_t _height, VkFormat _format, VkImageTiling _tiling, VkImageUsageFlags _usage)
	{
		VkImageCreateInfo imageCreateInfo;
		imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageCreateInfo.pNext = nullptr;
		imageCreateInfo.flags = 0;
		imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format = _format;
		imageCreateInfo.extent.width = _width;
		imageCreateInfo.extent.height = _height;
		imageCreateInfo.extent.depth = 1;
		imageCreateInfo.mipLevels = 1;
		imageCreateInfo.arrayLayers = 1;
		imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.tiling = _tiling;
		imageCreateInfo.usage = _usage;
		imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.queueFamilyIndexCount = 0;
		imageCreateInfo.pQueueFamilyIndices = nullptr;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		return imageCreateInfo;
	}

	static inline VkSamplerCreateInfo GetVkSamplerCreateInfo(VkFilter _magFilter, VkFilter _minFilter, VkSamplerMipmapMode _mipmapMode, VkSamplerAddressMode _addressModeU, VkSamplerAddressMode _addressModeV, VkSamplerAddressMode _addressModeW, float _mipLodBias, VkBool32 _anisotropyEnable, float _maxAnisotropy, VkBool32 _compareEnable, VkCompareOp _compareOp, float _minLod, float _maxLod, VkBorderColor _borderColor, VkBool32 _unnormalizedCoordinates)
	{
		VkSamplerCreateInfo samplerCreateInfo;
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.pNext = nullptr;
		samplerCreateInfo.flags = VK_RESERVED_FOR_FUTURE_USE;
		samplerCreateInfo.magFilter = _magFilter;
		samplerCreateInfo.minFilter = _minFilter;
		samplerCreateInfo.mipmapMode = _mipmapMode;
		samplerCreateInfo.addressModeU = _addressModeU;
		samplerCreateInfo.addressModeV = _addressModeV;
		samplerCreateInfo.addressModeW = _addressModeW;
		samplerCreateInfo.mipLodBias = _mipLodBias;
		samplerCreateInfo.anisotropyEnable = _anisotropyEnable;
		samplerCreateInfo.maxAnisotropy = _maxAnisotropy;
		samplerCreateInfo.compareEnable = _compareEnable;
		samplerCreateInfo.compareOp = _compareOp;
		samplerCreateInfo.minLod = _minLod;
		samplerCreateInfo.maxLod = _maxLod;
		samplerCreateInfo.borderColor = _borderColor;
		samplerCreateInfo.unnormalizedCoordinates = _unnormalizedCoordinates;
		return samplerCreateInfo;
	}

	static inline VkDescriptorPoolSize GetVkDescriptorPoolSize(VkDescriptorType _type, uint32_t _descriptorCount)
	{
		VkDescriptorPoolSize descriptorPoolSize;
		descriptorPoolSize.type = _type;
		descriptorPoolSize.descriptorCount = _descriptorCount;
		return descriptorPoolSize;
	}
	static inline VkDescriptorSetAllocateInfo GetVkDescriptorSetAllocateInfo(VkDescriptorPool _descriptorPool, uint32_t _descriptorSetCount, const VkDescriptorSetLayout* _pSetLayouts)
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo;
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.pNext = nullptr;
		descriptorSetAllocateInfo.descriptorPool = _descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = _descriptorSetCount;
		descriptorSetAllocateInfo.pSetLayouts = _pSetLayouts;
		return descriptorSetAllocateInfo;
	}

	static inline VkDescriptorBufferInfo GetVkDescriptorBufferInfo(VkBuffer _buffer, VkDeviceSize _offset, VkDeviceSize _range)
	{
		VkDescriptorBufferInfo descriptorBufferInfo;
		descriptorBufferInfo.buffer = _buffer;
		descriptorBufferInfo.offset = _offset;
		descriptorBufferInfo.range = _range;
		return descriptorBufferInfo;
	}
	static inline VkDescriptorImageInfo GetVkDescriptorImageInfo(VkSampler _sampler, VkImageView _imageView, VkImageLayout _imageLayout)
	{
		VkDescriptorImageInfo descriptorImageInfo;
		descriptorImageInfo.sampler = _sampler;
		descriptorImageInfo.imageView = _imageView;
		descriptorImageInfo.imageLayout = _imageLayout;
		return descriptorImageInfo;
	}

}

#endif