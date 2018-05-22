#ifndef	VERTEX_H
#define VERTEX_H

#include "Error.h"

// Order
/*
position
uv
color
normal
tangent
bitangent
index[n]
weight[n]
*/

namespace Vertex
{
	struct ATTRIBUTE
	{
		enum attribute : uint16_t
		{
			UNKNOWN = 0x0000,

			POS1 = 0x0001,
			POS2 = 0x0002,
			POS3 = POS1 | POS2,
			UV = 0x0004,
			NORMAL = 0x0008,
			TANGENT = 0x0010,
			BITANGENT = 0x0020,

			BONE16_1 = 0x0040,
			BONE16_2 = 0x0080,
			BONE16_3 = BONE16_2 | BONE16_1,
			BONE16_4 = 0x0100,
			BONE16_5 = BONE16_4 | BONE16_1,
			BONE16_6 = BONE16_4 | BONE16_2,
			BONE16_7 = BONE16_4 | BONE16_2 | BONE16_1,

			BONE16_MASK = BONE16_1 | BONE16_2 | BONE16_4,

			COLOR1 = 0x0200,
			COLOR2 = 0x0400,
			COLOR3 = COLOR1 | COLOR2,
			COLOR4 = 0x0800,

			COLOR5 = COLOR4 | COLOR1,
			COLOR6 = COLOR4 | COLOR2,
			COLOR7 = COLOR4 | COLOR2 | COLOR1,

			MAT4 = 0x1000,
		};
	};
	struct DATA
	{
		enum type : uint16_t
		{
			UNKNOWN = ATTRIBUTE::UNKNOWN,

			POS1 = ATTRIBUTE::POS1,
			POS2 = ATTRIBUTE::POS2,
			POS3 = ATTRIBUTE::POS3,
			UV = ATTRIBUTE::UV,
			NORMAL = ATTRIBUTE::NORMAL,
			TANGENT = ATTRIBUTE::TANGENT,
			BITANGENT = ATTRIBUTE::BITANGENT,

			BONE16_1 = ATTRIBUTE::BONE16_1,
			BONE16_2 = ATTRIBUTE::BONE16_2,
			BONE16_3 = ATTRIBUTE::BONE16_3,
			BONE16_4 = ATTRIBUTE::BONE16_4,

			COLOR1 = ATTRIBUTE::COLOR1,
			COLOR2 = ATTRIBUTE::COLOR2,
			COLOR3 = ATTRIBUTE::COLOR3,
			COLOR4 = ATTRIBUTE::COLOR4,

			MAT4 = ATTRIBUTE::MAT4,

			POS2_UV = POS2 | UV,
			POS3_UV_NORMAL_COLOR3 = POS3 | UV | NORMAL | COLOR3,
			POS3_UV_NORMAL_BONE16_4 = POS3 | UV | NORMAL | BONE16_4,

			UV_NORMAL_COLOR3 = UV | NORMAL | COLOR3,
		};
	};
	struct INDEX
	{
		enum type : uint8_t
		{
			UINT16,
			UINT32,
		};

		static inline size_t GetSize(Vertex::INDEX::type _indextype)
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

	struct DATATYPE
	{
		enum type : uint8_t
		{
			FLOAT = 0x01,
			VEC2 = 0x02,
			VEC3 = 0x04,
			MAT4 = 0x08,
		};
	};
	struct STRUCTURE
	{
		std::vector<DATATYPE> types;
	};

	static inline size_t GetDatatypeStride(DATA::type _vertexDataType)
	{
		return
			((_vertexDataType & DATA::POS1) == DATA::POS1) * sizeof(float) * 1 +
			((_vertexDataType & DATA::POS2) == DATA::POS2) * sizeof(float) * 2 +
			((_vertexDataType & DATA::UV) == DATA::UV) * sizeof(float) * 2 +
			((_vertexDataType & DATA::NORMAL) == DATA::NORMAL) * sizeof(float) * 3 +
			((_vertexDataType & DATA::TANGENT) == DATA::TANGENT) * sizeof(float) * 3 +
			((_vertexDataType & DATA::BITANGENT) == DATA::BITANGENT) * sizeof(float) * 3 +

			((_vertexDataType & DATA::BONE16_1) == DATA::BONE16_1)	* (sizeof(float) + sizeof(uint16_t)) * 1 +
			((_vertexDataType & DATA::BONE16_2) == DATA::BONE16_2)	* (sizeof(float) + sizeof(uint16_t)) * 2 +
			((_vertexDataType & DATA::BONE16_4) == DATA::BONE16_4)	* (sizeof(float) + sizeof(uint16_t)) * 4 +

			((_vertexDataType & DATA::COLOR1) == DATA::COLOR1) * sizeof(float) * 1 +
			((_vertexDataType & DATA::COLOR2) == DATA::COLOR2) * sizeof(float) * 2 +
			((_vertexDataType & DATA::COLOR4) == DATA::COLOR4) * sizeof(float) * 4 +

			((_vertexDataType & DATA::MAT4) == DATA::MAT4) * sizeof(Math3D::Mat4) * 1;
	}
	static inline size_t GetAttributeOffset(DATA::type _vertexDataType, ATTRIBUTE::attribute _attribute)
	{
		size_t offset = 0;

		// POS1
		if (_attribute == DATA::POS1)
			return offset;
		if ((_vertexDataType & DATA::POS1) == DATA::POS1)
			offset += sizeof(float) * 1;
		// POS2
		if (_attribute == DATA::DATA::POS2)
			return offset;
		if ((_vertexDataType & DATA::POS2) == DATA::POS2)
			offset += sizeof(float) * 2;
		// UV
		if (_attribute == DATA::DATA::UV)
			return offset;
		if ((_vertexDataType & DATA::UV) == DATA::UV)
			offset += sizeof(float) * 2;
		// NORMAL
		if (_attribute == DATA::NORMAL)
			return offset;
		if ((_vertexDataType & DATA::NORMAL) == DATA::NORMAL)
			offset += sizeof(float) * 3;
		// TANGENT
		if (_attribute == DATA::TANGENT)
			return offset;
		if ((_vertexDataType & DATA::TANGENT) == DATA::TANGENT)
			offset += sizeof(float) * 3;
		// BITANGENT
		if (_attribute == DATA::BITANGENT)
			return offset;
		if ((_vertexDataType & DATA::BITANGENT) == DATA::BITANGENT)
			offset += sizeof(float) * 3;

		// BONE16_1
		if (_attribute == DATA::BONE16_1)
			return offset;
		if ((_vertexDataType & DATA::BONE16_1) == DATA::BONE16_1)
			offset += (sizeof(uint16_t) + sizeof(float)) * 1;
		// BONE16_2
		if (_attribute == DATA::BONE16_2)
			return offset;
		if ((_vertexDataType & DATA::BONE16_2) == DATA::BONE16_2)
			offset += (sizeof(uint16_t) + sizeof(float)) * 2;
		// BONE16_4
		if (_attribute == DATA::BONE16_4)
			return offset;
		if ((_vertexDataType & DATA::BONE16_4) == DATA::BONE16_4)
			offset += (sizeof(uint16_t) + sizeof(float)) * 4;

		// COLOR1
		if (_attribute == DATA::COLOR1)
			return offset;
		if ((_vertexDataType & DATA::COLOR1) == DATA::COLOR1)
			offset += sizeof(float) * 1;
		// COLOR2
		if (_attribute == DATA::COLOR2)
			return offset;
		if ((_vertexDataType & DATA::COLOR2) == DATA::COLOR2)
			offset += sizeof(float) * 2;
		// COLOR4
		if (_attribute == DATA::COLOR4)
			return offset;
		if ((_vertexDataType & DATA::COLOR4) == DATA::COLOR4)
			offset += sizeof(float) * 4;

		//MAT4

		if ((_vertexDataType & DATA::MAT4) == DATA::MAT4)
			offset += sizeof(Math3D::Mat4) * 1;

		Error::Report("MISSING ATTRIBUTE");
		return ~0U;
	}

	static inline void* GetVertexAddress(void* _bufferAddress, DATA::type _vertexDataType, size_t _vertexIndex)
	{
		return (void*)((size_t)_bufferAddress + GetDatatypeStride(_vertexDataType)*_vertexIndex);
	}
	static inline void* GetAttributeAddress(void* _vertexAddress, DATA::type _vertexDataType, ATTRIBUTE::attribute _attribute)
	{
		return (void*)((size_t)_vertexAddress + GetAttributeOffset(_vertexDataType, _attribute));
	}

	static inline void* CreateVertexBuffer(DATA::type _vertexDataType, size_t _vertexCount)
	{
		size_t vertexDataSize = GetDatatypeStride(_vertexDataType);
		return new uint8_t[vertexDataSize * _vertexCount];
	}
	static inline void* CreateIndexBuffer(INDEX::type _indexType, size_t _indexCount)
	{
		size_t indexDataSize = INDEX::GetSize(_indexType);
		return new uint8_t[indexDataSize * _indexCount];
	}

	namespace BONE16
	{
		static inline ATTRIBUTE::attribute GetVertexBoneAttribute(DATA::type _vertexDatatype)
		{
			return (ATTRIBUTE::attribute)(_vertexDatatype & ATTRIBUTE::BONE16_MASK);
		}
		static inline size_t GetVertexBoneCount(DATA::type _vertexDatatype)
		{
			size_t bone16_1 = ATTRIBUTE::BONE16_1;
			size_t bone16_N = GetVertexBoneAttribute(_vertexDatatype);
			while (bone16_1 != 1)
			{
				bone16_1 = bone16_1 >> 1;
				bone16_N = bone16_N >> 1;
			}

			return bone16_N;
		}
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
		static void SetAttribute(void* _bufferAddress, DATA::type _vertexDatatype, size_t _vertexIndex, size_t _index, uint16_t _boneIndex, float _boneWeight)
		{
			void* vertexAddress = GetVertexAddress(_bufferAddress, _vertexDatatype, _vertexIndex);
			void* attributeAddress = GetAttributeAddress(vertexAddress, _vertexDatatype, GetVertexBoneAttribute(_vertexDatatype));
			size_t weightCount = GetVertexBoneCount(_vertexDatatype);
			size_t indexStride = sizeof(uint16_t) * weightCount;

			uint16_t* vertexIndices = (uint16_t*)attributeAddress;
			float* vertexWeights = (float*)((size_t)vertexIndices + indexStride);

			vertexIndices[_index] = _boneIndex;
			vertexWeights[_index] = _boneWeight;
		}
		static void SetLeastRelevantAttribute(void* _bufferAddress, DATA::type _vertexDatatype, size_t _vertexIndex, uint16_t _boneIndex, float _boneWeight)
		{
			void* vertexAddress = GetVertexAddress(_bufferAddress, _vertexDatatype, _vertexIndex);
			void* attributeAddress = GetAttributeAddress(vertexAddress, _vertexDatatype, GetVertexBoneAttribute(_vertexDatatype));
			size_t weightCount = GetVertexBoneCount(_vertexDatatype);
			size_t leastRelevant = Vertex::BONE16::GetIndexOfLeastRelevant(attributeAddress, weightCount);

			size_t indexStride = sizeof(uint16_t) * weightCount;

			uint16_t* vertexIndices = (uint16_t*)attributeAddress;
			float* vertexWeights = (float*)((size_t)vertexIndices + indexStride);

			vertexIndices[leastRelevant] = _boneIndex;
			vertexWeights[leastRelevant] = _boneWeight;
		}
	};
}

#endif