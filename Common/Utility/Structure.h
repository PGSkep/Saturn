#ifndef	STRUCTURE_H
#define STRUCTURE_H

#include <stdint.h>
#include <string>
#include <vector>

#include "../../Common/Utility/Error.h"

class Structure
{
public:
	struct DATATYPE
	{
		enum datatype
		{
			UINT8,
			UINT16,
			UINT32,
			UINT64,

			INT8,
			INT16,
			INT32,
			INT64,

			FLOAT,
			DOUBLE,
		};

		static inline size_t GetSize(datatype _datatype)
		{
			switch (_datatype)
			{
			case UINT8:
				return sizeof(uint8_t);
			case UINT16:
				return sizeof(uint16_t);
			case UINT32:
				return sizeof(uint32_t);
			case UINT64:
				return sizeof(uint64_t);

			case INT8:
				return sizeof(int8_t);
			case INT16:
				return sizeof(int16_t);
			case INT32:
				return sizeof(int32_t);
			case INT64:
				return sizeof(int64_t);

			case FLOAT:
				return sizeof(float);
			case DOUBLE:
				return sizeof(double);
			default:
				return 0;
			}
		}
	};
	struct Data
	{
		std::string			name;
		DATATYPE::datatype	datatype;
		size_t				count;

		static inline Data Get(std::string _name, DATATYPE::datatype _datatype, size_t _count)
		{
			Data data;

			data.name = _name;
			data.datatype = _datatype;
			data.count = _count;

			return data;
		}
		size_t GetSize()
		{
			return DATATYPE::GetSize(datatype) * count;
		}
	};

private:
	std::vector<Data> datas;
	size_t count;

public:
	std::vector<Data> GetDatas()
	{
		return datas;
	}
	size_t GetCount()
	{
		return count;
	}

	void SetDatas(std::vector<Data> _datas)
	{
		datas = _datas;
	}
	void SetCount(size_t _count)
	{
		count = _count;
	}

	size_t GetStride()
	{
		size_t size = 0;

		for (size_t i = 0; i != datas.size(); ++i)
			size += datas[i].GetSize();

		return size;
	}
	size_t GetSize()
	{
		return GetStride() * count;
	}

	size_t GetDataAddressOffset(const char* _name)
	{
		size_t offset = 0;

		bool found = false;
		for (size_t i = 0; i != datas.size(); ++i)
		{
			if (strcmp(_name, datas[i].name.c_str()) != 0)
			{
				offset += datas[i].GetSize();
			}
			else
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			Error::Report(std::string("Structure does not contain data named: ") + _name);
			return -1;
		}

		return offset;
	}
	size_t GetIndexAddressOffset(size_t _index)
	{
		return GetStride() * _index;
	}

	static inline Structure Get(size_t _count, std::vector<Data> _datas)
	{
		Structure structure;
		structure.count = _count;
		structure.datas = _datas;
		return structure;
	}
};

class StructuredBuffer
{
public:
	Structure structure;

	void* data;
	size_t dataSize;

	void* GetDataAddress(size_t _index, const char* _name)
	{
		return (void*)((size_t)data + structure.GetIndexAddressOffset(_index) + structure.GetDataAddressOffset(_name));
	}

	void Init(Structure _structure)
	{
		structure = _structure;
		dataSize = structure.GetSize();
		data = new uint8_t[dataSize];
	}
	void Clear()
	{
		delete[] data;
		dataSize = 0;
	}
};

#endif