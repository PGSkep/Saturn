#ifndef MEMORY_MAP_H
#define MEMORY_MAP_H

#include <string>
#include <map>

class MemoryMap
{
public:
	struct MEMORY_STATE
	{
		enum MemoryState
		{
			FREE,		// data is free for allocation
			MOVING,		// data was moved but not freed (data is still valid and needs to be flagged to be freed)
			GARBAGE,	// data is allocated, but not yet filled with data
			VALID,		// data is allocated and filled with data
		};
	};
	typedef size_t Offset;
	typedef size_t Size;
	typedef void* Userdata;
	struct MemoryInfo
	{
		Offset offset;
		Size size;

		static inline MemoryInfo Get(Offset _offset, Size _size)
		{
			return { _offset, _size };
		}
	};

	struct BufferNode
	{
		std::string					name;
		MemoryInfo					memoryInfo;
		MEMORY_STATE::MemoryState	state;
		BufferNode*					prev;
		BufferNode*					next;
		Userdata					userdata;

		static inline BufferNode Get(std::string _name, MemoryInfo _memoryInfo, MEMORY_STATE::MemoryState _state, BufferNode* _prev, BufferNode* _next, void* _userdata)
		{
			return { _name, _memoryInfo, _state, _prev, _next, _userdata };
		}
	};
	struct PoolNode
	{
		std::string							name;
		MemoryInfo							memoryInfo = { 0, 0 };
		size_t								alignment = 0;
		MEMORY_STATE::MemoryState			state = MEMORY_STATE::FREE;
		PoolNode*							prev = nullptr;
		PoolNode*							next = nullptr;
		BufferNode*							bufferNodeHead = nullptr;
		bool								ownsBufferNodeUserdata = false;
		size_t								usedSpace = 0;
		std::map<std::string, BufferNode*>	bufferPtrs;
		Userdata							userdata = nullptr;

		static inline PoolNode Get(std::string _name, MemoryInfo _memoryInfo, size_t _alignment, MEMORY_STATE::MemoryState _state, PoolNode* _prev, PoolNode* _next, BufferNode* _bufferNodeHead, bool _ownsNodeUserdata, size_t _usedSpace, std::map<std::string, BufferNode*> _bufferPtrs, Userdata _userdata)
		{
			return { _name, _memoryInfo, _alignment, _state, _prev, _next, _bufferNodeHead, _ownsNodeUserdata, _usedSpace, _bufferPtrs, _userdata };
		}

		void InitHead(bool _ownsNodeUserdata);

		BufferNode* AllocateBuffer(const char* _name, size_t _requestedSpace, Userdata _userdata);
		BufferNode* GetBuffer(const char* _name);
		Userdata FreeBuffer(BufferNode* _buffer);

		//void ShutDown();
	};

private:
	PoolNode* poolNodeHead = nullptr;
	bool ownsPoolNodeUserdata = false;
	size_t availableSpace = 0;
	size_t pooledSpace = 0;
	std::map<std::string, PoolNode*> poolPtrs;

public:
	void InitHead(size_t _availableSpace, bool _ownsNodeUserdata);

	PoolNode* AllocatePool(const char* _name, size_t _requestedSpace, size_t _alignment, Userdata _userdata, bool _ownsNodeUserdata);
	PoolNode* GetPool(const char* _name);
	Userdata FreePool(PoolNode* _pool);

	void ShutDown();

	template <class T>
	static T* GetNewT(T _type)
	{
		T* t = new T;
		*t = _type;
		return t;
	}

	static size_t GetAlignedSize(size_t _size, size_t _alignment)
	{
		if (_alignment == 0)
			return _size;
		return _size + (_alignment - (_size % _alignment));
	}
};

#endif