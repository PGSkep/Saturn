#ifndef	VERTEX2_H
#define VERTEX2_H

#include "Error.h"
#include "Math3D.h"

#include <string>

namespace Vertex
{
	struct DATATYPE
	{
		enum datatype : uint32_t
		{
			UNKNOWN		= 0x00000000,

			FLOAT_1		= 0x00000001,
			FLOAT_2		= 0x00000002,
			FLOAT_3		= FLOAT_1 | FLOAT_2,
			FLOAT_4		= 0x00000004,

			UINT16_1	= 0x00000008,
			UINT16_2	= 0x00000010,
			UINT16_3	= UINT16_1 | UINT16_2,
			UINT16_4	= 0x00000020,

			BONE_16_1	= 0x00000040,
			BONE_16_2	= 0x00000080,
			BONE_16_3	= BONE_16_1 | BONE_16_2,
			BONE_16_4	= 0x00000100,

			MAT4		= 0x00000200,
		};

		static inline size_t GetSize(DATATYPE::datatype _datatype)
		{
			switch (_datatype)
			{
			case FLOAT_1:	return sizeof(float) * 1;
			case FLOAT_2:	return sizeof(float) * 2;
			case FLOAT_3:	return sizeof(float) * 3;
			case FLOAT_4:	return sizeof(float) * 4;

			case UINT16_1:	return sizeof(uint16_t) * 1;
			case UINT16_2:	return sizeof(uint16_t) * 2;
			case UINT16_3:	return sizeof(uint16_t) * 3;
			case UINT16_4:	return sizeof(uint16_t) * 4;

			case MAT4:		return sizeof(Math3D::Mat4);
			}

			return -1;
		}
	};
	struct ATTRIBUTE
	{
		enum attribute
		{
			UNKNOWN,

			POS1,
			POS2,
			POS3,

			UV,
			NORMAL,
			TANGENT,
			BITANGENT,

			COLOR1,
			COLOR2,
			COLOR3,
			COLOR4,

			BONE16_1,
			BONE16_2,
			BONE16_3,
			BONE16_4,

			ROT3,
			SCALE1,
			LIGHTCOUNT,
			BONECOUNT,
		};

		static inline DATATYPE::datatype GetDatatype(ATTRIBUTE::attribute _attribute)
		{
			switch (_attribute)
			{
			case POS1:			return DATATYPE::FLOAT_1;
			case POS2:			return DATATYPE::FLOAT_2;
			case POS3:			return DATATYPE::FLOAT_3;

			case UV:			return DATATYPE::FLOAT_2;
			case NORMAL:		return DATATYPE::FLOAT_3;
			case TANGENT:		return DATATYPE::FLOAT_3;
			case BITANGENT:		return DATATYPE::FLOAT_3;

			case COLOR1:		return DATATYPE::FLOAT_1;
			case COLOR2:		return DATATYPE::FLOAT_2;
			case COLOR3:		return DATATYPE::FLOAT_3;
			case COLOR4:		return DATATYPE::FLOAT_4;

			case BONE16_1:		return DATATYPE::BONE_16_1;
			case BONE16_2:		return DATATYPE::BONE_16_2;
			case BONE16_3:		return DATATYPE::BONE_16_3;
			case BONE16_4:		return DATATYPE::BONE_16_4;

			case ROT3:	return DATATYPE::FLOAT_3;
			case SCALE1:	return DATATYPE::FLOAT_1;
			case LIGHTCOUNT:	return DATATYPE::UINT16_1;
			case BONECOUNT:		return DATATYPE::UINT16_1;
			}

			return DATATYPE::UNKNOWN;
		}
		static inline size_t GetSize(ATTRIBUTE::attribute _attribute)
		{
			switch (_attribute)
			{
			case POS1:			return DATATYPE::GetSize(GetDatatype(POS1));
			case POS2:			return DATATYPE::GetSize(GetDatatype(POS2));
			case POS3:			return DATATYPE::GetSize(GetDatatype(POS3));

			case UV:			return DATATYPE::GetSize(GetDatatype(UV));
			case NORMAL:		return DATATYPE::GetSize(GetDatatype(NORMAL));
			case TANGENT:		return DATATYPE::GetSize(GetDatatype(TANGENT));
			case BITANGENT:		return DATATYPE::GetSize(GetDatatype(BITANGENT));

			case COLOR1:		return DATATYPE::GetSize(GetDatatype(COLOR1));
			case COLOR2:		return DATATYPE::GetSize(GetDatatype(COLOR2));
			case COLOR3:		return DATATYPE::GetSize(GetDatatype(COLOR3));
			case COLOR4:		return DATATYPE::GetSize(GetDatatype(COLOR4));

			case BONE16_1:		return DATATYPE::GetSize(GetDatatype(BONE16_1));
			case BONE16_2:		return DATATYPE::GetSize(GetDatatype(BONE16_2));
			case BONE16_3:		return DATATYPE::GetSize(GetDatatype(BONE16_3));
			case BONE16_4:		return DATATYPE::GetSize(GetDatatype(BONE16_4));

			case ROT3:	return DATATYPE::GetSize(GetDatatype(ROT3));
			case SCALE1:	return DATATYPE::GetSize(GetDatatype(SCALE1));
			case LIGHTCOUNT:	return DATATYPE::GetSize(GetDatatype(LIGHTCOUNT));
			case BONECOUNT:		return DATATYPE::GetSize(GetDatatype(BONECOUNT));
			}

			return -1;
		}
	};

	struct VERTEXTYPE
	{
		enum vertextype
		{
			POS1			= 0x00000001,
			POS2			= 0x00000002,
			POS3			= 0x00000004,

			UV				= 0x00000008,
			NORMAL			= 0x00000010,
			TANGENT			= 0x00000020,
			BITANGENT		= 0x00000040,

			COLOR1			= 0x00000080,
			COLOR2			= 0x00000100,
			COLOR3			= 0x00000200,
			COLOR4			= 0x00000400,

			BONE16_1		= 0x00000800,
			BONE16_2		= 0x00001000,
			BONE16_3		= 0x00002000,
			BONE16_4		= 0x00004000,

			ROT3			= 0x00008000,
			SCALE1			= 0x00010000,
			LIGHTCOUNT		= 0x00020000,
			BONECOUNT		= 0x00040000,

			POS3_UV_NORMAL_COLOR3			= POS3	| UV		| NORMAL	| COLOR3,
			UV_NORMAL						= UV	| NORMAL,
			UV_NORMAL_COLOR3				= UV	| NORMAL	| COLOR3,

			POS3_COLOR3_ROT3_SCALE1_LIGHTCOUNT = POS3 | COLOR3 | ROT3 | SCALE1 | LIGHTCOUNT,
		};

		static inline size_t GetStride(VERTEXTYPE::vertextype _vertextype)
		{
			size_t stride = 0;

			if ((_vertextype & POS1) == POS1)				stride += ATTRIBUTE::GetSize(ATTRIBUTE::POS1);
			if ((_vertextype & POS2) == POS2)				stride += ATTRIBUTE::GetSize(ATTRIBUTE::POS2);
			if ((_vertextype & POS3) == POS3)				stride += ATTRIBUTE::GetSize(ATTRIBUTE::POS3);

			if ((_vertextype & UV) == UV)					stride += ATTRIBUTE::GetSize(ATTRIBUTE::UV);
			if ((_vertextype & NORMAL) == NORMAL)			stride += ATTRIBUTE::GetSize(ATTRIBUTE::NORMAL);
			if ((_vertextype & TANGENT) == TANGENT)			stride += ATTRIBUTE::GetSize(ATTRIBUTE::TANGENT);
			if ((_vertextype & BITANGENT) == BITANGENT)		stride += ATTRIBUTE::GetSize(ATTRIBUTE::BITANGENT);

			if ((_vertextype & COLOR1) == COLOR1)			stride += ATTRIBUTE::GetSize(ATTRIBUTE::COLOR1);
			if ((_vertextype & COLOR2) == COLOR2)			stride += ATTRIBUTE::GetSize(ATTRIBUTE::COLOR2);
			if ((_vertextype & COLOR3) == COLOR3)			stride += ATTRIBUTE::GetSize(ATTRIBUTE::COLOR3);
			if ((_vertextype & COLOR4) == COLOR4)			stride += ATTRIBUTE::GetSize(ATTRIBUTE::COLOR4);

			if ((_vertextype & BONE16_1) == BONE16_1)		stride += ATTRIBUTE::GetSize(ATTRIBUTE::BONE16_1);
			if ((_vertextype & BONE16_2) == BONE16_2)		stride += ATTRIBUTE::GetSize(ATTRIBUTE::BONE16_2);
			if ((_vertextype & BONE16_3) == BONE16_3)		stride += ATTRIBUTE::GetSize(ATTRIBUTE::BONE16_3);
			if ((_vertextype & BONE16_4) == BONE16_4)		stride += ATTRIBUTE::GetSize(ATTRIBUTE::BONE16_4);

			if ((_vertextype & ROT3) == ROT3)	stride += ATTRIBUTE::GetSize(ATTRIBUTE::ROT3);
			if ((_vertextype & SCALE1) == SCALE1)	stride += ATTRIBUTE::GetSize(ATTRIBUTE::SCALE1);
			if ((_vertextype & LIGHTCOUNT) == LIGHTCOUNT)	stride += ATTRIBUTE::GetSize(ATTRIBUTE::LIGHTCOUNT);
			if ((_vertextype & BONECOUNT) == BONECOUNT)		stride += ATTRIBUTE::GetSize(ATTRIBUTE::BONECOUNT);

			return stride;
		}
		static inline size_t GetBoneCount(VERTEXTYPE::vertextype _vertextype)
		{
			if ((_vertextype & BONE16_1) == BONE16_1)
				return 1;
			if ((_vertextype & BONE16_2) == BONE16_2)
				return 2;
			if ((_vertextype & BONE16_3) == BONE16_3)
				return 3;
			if ((_vertextype & BONE16_4) == BONE16_4)
				return 4;

			return 0;
		}
		static inline ATTRIBUTE::attribute GetBoneAttribute(VERTEXTYPE::vertextype _vertextype)
		{
			if ((_vertextype & BONE16_1) == BONE16_1)
				return ATTRIBUTE::BONE16_1;
			if ((_vertextype & BONE16_2) == BONE16_2)
				return ATTRIBUTE::BONE16_2;
			if ((_vertextype & BONE16_3) == BONE16_3)
				return ATTRIBUTE::BONE16_3;
			if ((_vertextype & BONE16_4) == BONE16_4)
				return ATTRIBUTE::BONE16_4;

			return ATTRIBUTE::UNKNOWN;
		}
	};

	struct STRUCTURE
	{
		std::vector<ATTRIBUTE::attribute> attributes;
		VERTEXTYPE::vertextype type;

		static inline size_t GetStride(STRUCTURE _structure)
		{
			size_t stride = 0;

			for (size_t i = 0; i != _structure.attributes.size(); ++i)
				stride += DATATYPE::GetSize(ATTRIBUTE::GetDatatype(_structure.attributes[i]));

			return stride;
		}
		static inline size_t GetOffset(STRUCTURE _structure, ATTRIBUTE::attribute _attribute)
		{
			size_t offset = 0;

			for (size_t i = 0; ; ++i)
			{
				if (i == _structure.attributes.size())
				{
					Error::Report("Structure name missing");
					return -1;
				}

				if (_structure.attributes[i] == _attribute)
					break;

				offset += DATATYPE::GetSize(ATTRIBUTE::GetDatatype(_structure.attributes[i]));
			}

			return offset;
		}

		static inline STRUCTURE GetStructure(VERTEXTYPE::vertextype _vertextype)
		{
			STRUCTURE structure;
			structure.type = _vertextype;

			if ((_vertextype & VERTEXTYPE::POS1) == VERTEXTYPE::POS1)				structure.attributes.push_back(ATTRIBUTE::POS1);
			if ((_vertextype & VERTEXTYPE::POS2) == VERTEXTYPE::POS2)				structure.attributes.push_back(ATTRIBUTE::POS2);
			if ((_vertextype & VERTEXTYPE::POS3) == VERTEXTYPE::POS3)				structure.attributes.push_back(ATTRIBUTE::POS3);

			if ((_vertextype & VERTEXTYPE::UV) == VERTEXTYPE::UV)					structure.attributes.push_back(ATTRIBUTE::UV);
			if ((_vertextype & VERTEXTYPE::NORMAL) == VERTEXTYPE::NORMAL)			structure.attributes.push_back(ATTRIBUTE::NORMAL);
			if ((_vertextype & VERTEXTYPE::TANGENT) == VERTEXTYPE::TANGENT)			structure.attributes.push_back(ATTRIBUTE::TANGENT);
			if ((_vertextype & VERTEXTYPE::BITANGENT) == VERTEXTYPE::BITANGENT)		structure.attributes.push_back(ATTRIBUTE::BITANGENT);

			if ((_vertextype & VERTEXTYPE::COLOR1) == VERTEXTYPE::COLOR1)			structure.attributes.push_back(ATTRIBUTE::COLOR1);
			if ((_vertextype & VERTEXTYPE::COLOR2) == VERTEXTYPE::COLOR2)			structure.attributes.push_back(ATTRIBUTE::COLOR2);
			if ((_vertextype & VERTEXTYPE::COLOR3) == VERTEXTYPE::COLOR3)			structure.attributes.push_back(ATTRIBUTE::COLOR3);
			if ((_vertextype & VERTEXTYPE::COLOR4) == VERTEXTYPE::COLOR4)			structure.attributes.push_back(ATTRIBUTE::COLOR4);

			if ((_vertextype & VERTEXTYPE::BONE16_1) == VERTEXTYPE::BONE16_1)		structure.attributes.push_back(ATTRIBUTE::BONE16_1);
			if ((_vertextype & VERTEXTYPE::BONE16_2) == VERTEXTYPE::BONE16_2)		structure.attributes.push_back(ATTRIBUTE::BONE16_2);
			if ((_vertextype & VERTEXTYPE::BONE16_3) == VERTEXTYPE::BONE16_3)		structure.attributes.push_back(ATTRIBUTE::BONE16_3);
			if ((_vertextype & VERTEXTYPE::BONE16_4) == VERTEXTYPE::BONE16_4)		structure.attributes.push_back(ATTRIBUTE::BONE16_4);

			if ((_vertextype & VERTEXTYPE::ROT3) == VERTEXTYPE::ROT3)	structure.attributes.push_back(ATTRIBUTE::ROT3);
			if ((_vertextype & VERTEXTYPE::SCALE1) == VERTEXTYPE::SCALE1)	structure.attributes.push_back(ATTRIBUTE::SCALE1);
			if ((_vertextype & VERTEXTYPE::LIGHTCOUNT) == VERTEXTYPE::LIGHTCOUNT)	structure.attributes.push_back(ATTRIBUTE::LIGHTCOUNT);
			if ((_vertextype & VERTEXTYPE::BONECOUNT) == VERTEXTYPE::BONECOUNT)		structure.attributes.push_back(ATTRIBUTE::BONECOUNT);

			return structure;
		}

		static inline void* GetIndexAddress(void* _buffer, STRUCTURE _structure, size_t _index)
		{
			size_t address = (size_t)_buffer;
			size_t stride = GetStride(_structure);
			return (void*)(address + (stride * _index));
		}
		static inline void* GetAttributeAddress(void* _indexAddress, STRUCTURE _structure, ATTRIBUTE::attribute _attribute)
		{
			size_t indexAddress = (size_t)_indexAddress;
			size_t offset = GetOffset(_structure, _attribute);
			return (void*)(indexAddress + offset);
		}
	};
	
	struct INDEX
	{
		enum type : uint8_t
		{
			UINT16,
			UINT32,
		};

		static inline size_t GetSize(INDEX::type _indextype)
		{
			switch (_indextype)
			{
			case UINT16:
				return sizeof(uint16_t);
			case UINT32:
				return sizeof(uint32_t);
			default:
				Error::Report("MISSING INDEX TYPE");
				return 0;
			}
		}
	};

	static inline void* GetVertexAddress(void* _bufferAddress, VERTEXTYPE::vertextype _vertexDataType, size_t _vertexIndex)
	{
		return (void*)((size_t)_bufferAddress + VERTEXTYPE::GetStride(_vertexDataType)*_vertexIndex);
	}
	static inline void* GetAttributeAddress(void* _vertexAddress, VERTEXTYPE::vertextype _vertexDataType, ATTRIBUTE::attribute _attribute)
	{
		return (void*)((size_t)_vertexAddress + STRUCTURE::GetOffset(STRUCTURE::GetStructure(_vertexDataType), _attribute));
	}

	namespace BONE16
	{
		static inline size_t GetIndexOfLeastRelevant(void* _boneAddress, size_t _weightCount)
		{
			float* weightArr = (float*)((size_t)_boneAddress + sizeof(uint16_t)*_weightCount);

			size_t smallest = 0;
			float weight = 1.0f;
			for (size_t i = 0; i != _weightCount; ++i)
			{
				if (weightArr[i] == 0.0f)
					return i;

				if (weightArr[i] < weight)
				{
					weight = weightArr[i];
					smallest = i;
				}
			}

			return smallest;
		}
		static void SetAttribute(void* _bufferAddress, VERTEXTYPE::vertextype _vertexDatatype, size_t _vertexIndex, size_t _index, uint16_t _boneIndex, float _boneWeight)
		{
			void* vertexAddress = GetVertexAddress(_bufferAddress, _vertexDatatype, _vertexIndex);
			void* attributeAddress = GetAttributeAddress(vertexAddress, _vertexDatatype, VERTEXTYPE::GetBoneAttribute(_vertexDatatype));
			size_t weightCount = VERTEXTYPE::GetBoneCount(_vertexDatatype);
			size_t indexStride = sizeof(uint16_t) * weightCount;

			uint16_t* vertexIndices = (uint16_t*)attributeAddress;
			float* vertexWeights = (float*)((size_t)vertexIndices + indexStride);

			vertexIndices[_index] = _boneIndex;
			vertexWeights[_index] = _boneWeight;
		}
		static void SetLeastRelevantAttribute(void* _bufferAddress, VERTEXTYPE::vertextype _vertexDatatype, size_t _vertexIndex, uint16_t _boneIndex, float _boneWeight)
		{
			void* vertexAddress = GetVertexAddress(_bufferAddress, _vertexDatatype, _vertexIndex);
			void* attributeAddress = GetAttributeAddress(vertexAddress, _vertexDatatype, VERTEXTYPE::GetBoneAttribute(_vertexDatatype));
			size_t weightCount = VERTEXTYPE::GetBoneCount(_vertexDatatype);
			size_t leastRelevant = BONE16::GetIndexOfLeastRelevant(attributeAddress, weightCount);

			size_t indexStride = sizeof(uint16_t) * weightCount;

			uint16_t* vertexIndices = (uint16_t*)attributeAddress;
			float* vertexWeights = (float*)((size_t)vertexIndices + indexStride);

			vertexIndices[leastRelevant] = _boneIndex;
			vertexWeights[leastRelevant] = _boneWeight;
		}
	};

	static inline void* CreateVertexBuffer(VERTEXTYPE::vertextype _vertexDataType, size_t _vertexCount)
	{
		size_t vertexDataSize = VERTEXTYPE::GetStride(_vertexDataType);
		return new uint8_t[vertexDataSize * _vertexCount];
	}
	static inline void* CreateIndexBuffer(INDEX::type _indexType, size_t _indexCount)
	{
		size_t indexDataSize = INDEX::GetSize(_indexType);
		return new uint8_t[indexDataSize * _indexCount];
	}
};

#endif