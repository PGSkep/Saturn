#include "MemoryMap.h"

#include "../../Common/Utility/Error.h"

void MemoryMap::InitHead(size_t _availableSpace, bool _ownsPoolNodeUserdata)
{
	availableSpace = _availableSpace;
	ownsPoolNodeUserdata = _ownsPoolNodeUserdata;

	poolNodeHead = new PoolNode;
	*poolNodeHead = PoolNode::Get("Free", MemoryInfo::Get(0, _availableSpace), 0, MEMORY_STATE::FREE, nullptr, nullptr, nullptr, false, 0, {}, nullptr);
}

MemoryMap::PoolNode* MemoryMap::AllocatePool(const char* _name, size_t _requestedSpace, size_t _alignment, Userdata _userdata, bool _ownsBufferNodeUserdata)
{
	size_t alignedSpace = _requestedSpace;
	if (_alignment != 0)
		alignedSpace = _requestedSpace + (_requestedSpace % alignedSpace);

	// Check if name already exists
	std::map<std::string, PoolNode*>::iterator iter = poolPtrs.find(_name);
	if (iter != poolPtrs.end())
	{
		Error::Report(std::string("Pool ") + _name + " already exists");
		return nullptr;
	}

	// Get node to allocate from
	PoolNode* targetPool = poolNodeHead;
	while (targetPool != nullptr)
	{
		if (targetPool->state == MEMORY_STATE::FREE)
		{
			if (targetPool->memoryInfo.size >= alignedSpace)
				break;
			else
				targetPool = targetPool->next;
		}
		else
			targetPool = targetPool->next;
	}

	// If failed to find node to allocate from
	if (targetPool == nullptr)
	{
		Error::Report("Failed to allocate pool, could not find empty space with fitting size");
		return nullptr;
	}

	// If node does NOT needs to be split
	if (targetPool->memoryInfo.size != alignedSpace)
	{
		PoolNode* newPool = new PoolNode;
		*newPool = PoolNode::Get("Free", MemoryMap::MemoryInfo::Get(targetPool->memoryInfo.offset + alignedSpace, targetPool->memoryInfo.size - alignedSpace), _alignment, MEMORY_STATE::FREE, targetPool, targetPool->next, nullptr, false, 0, {}, nullptr);

		if (targetPool->next != nullptr)
			targetPool->next->prev = newPool;

		targetPool->memoryInfo.size -= newPool->memoryInfo.size;
		targetPool->next = newPool;
	}

	// Set pool values
	targetPool->name = _name;
	targetPool->state = MEMORY_STATE::VALID;
	targetPool->InitHead(_ownsBufferNodeUserdata);
	targetPool->userdata = _userdata;
	targetPool->alignment = _alignment;

	// Add pool size to map
	pooledSpace += alignedSpace;

	// Add pool to map
	poolPtrs[_name] = targetPool;

	return targetPool;
}
MemoryMap::PoolNode * MemoryMap::GetPool(const char * _name)
{
	std::map<std::string, PoolNode*>::iterator iter = poolPtrs.find(_name);
	if (iter == poolPtrs.end())
		return nullptr;
	else
		return iter->second;
}
MemoryMap::Userdata MemoryMap::FreePool(PoolNode* _pool)
{
	if (_pool == nullptr)
		Error::Report("Pool is nullptr");

	PoolNode* prev = _pool->prev;
	PoolNode* next = _pool->next;
	Userdata userdata = _pool->userdata;

	// Remove pool size from map
	pooledSpace -= _pool->memoryInfo.size;

	// Merge previous pool
	if (prev != nullptr)
	{
		if (prev->state == MEMORY_STATE::FREE)
		{
			// Add memoryInfo
			_pool->memoryInfo.offset -= prev->memoryInfo.size;
			_pool->memoryInfo.size += prev->memoryInfo.size;

			// Point
			_pool->prev = prev->prev;
			if (prev->prev != nullptr)
				prev->prev->next = _pool;

			// move head
			if (prev == poolNodeHead)
				poolNodeHead = _pool;

			delete prev;
		}
	}

	// Merge next pool
	if (next != nullptr)
	{
		if (next->state == MEMORY_STATE::FREE)
		{
			// Add memoryInfo
			_pool->memoryInfo.size += next->memoryInfo.size;

			// Point
			_pool->next = next->next;
			if (next->next != nullptr)
				next->next->prev = _pool;

			delete next;
		}
	}

	// Free pool
	_pool->name = "Free";
	_pool->state = MEMORY_STATE::FREE;

	// Remove pool from map
	poolPtrs.erase(_pool->name);

	return userdata;
}

void MemoryMap::ShutDown()
{
	// Free pools userdata
	if (ownsPoolNodeUserdata == true)
		for (std::map<std::string, PoolNode*>::iterator iter = poolPtrs.begin(); iter != poolPtrs.end(); ++iter)
			delete iter->second->userdata;

	poolPtrs.clear();

	// Free pool nodes
	PoolNode* tempPool = poolNodeHead;
	while (tempPool != nullptr)
	{
		// Free pool buffers userdatas
		if (tempPool->ownsBufferNodeUserdata == true)
			for (std::map<std::string, BufferNode*>::iterator iter = tempPool->bufferPtrs.begin(); iter != tempPool->bufferPtrs.end(); ++iter)
				delete iter->second->userdata;

		// Free pool buffers nodes
		BufferNode* tempBuffer = tempPool->bufferNodeHead;
		while (tempBuffer != nullptr)
		{
			tempPool->bufferNodeHead = tempPool->bufferNodeHead->next;
			delete tempBuffer;
			tempBuffer = tempPool->bufferNodeHead;
		}

		// Free pool node
		poolNodeHead = poolNodeHead->next;
		delete tempPool;
		tempPool = poolNodeHead;
	}
}

void MemoryMap::PoolNode::InitHead(bool _ownsBufferNodeUserdata)
{
	bufferNodeHead = new BufferNode;
	*bufferNodeHead = BufferNode::Get("Free", MemoryInfo::Get(0, memoryInfo.size), MEMORY_STATE::FREE, nullptr, nullptr, nullptr);

	ownsBufferNodeUserdata = _ownsBufferNodeUserdata;
}

MemoryMap::BufferNode* MemoryMap::PoolNode::AllocateBuffer(const char * _name, size_t _requestedSpace, Userdata _userdata)
{
	size_t alignedSize = GetAlignedSize(_requestedSpace, alignment);

	// Check if name already exists
	std::map<std::string, BufferNode*>::iterator iter = bufferPtrs.find(_name);
	if (iter != bufferPtrs.end())
	{
		Error::Report(std::string("Pool ") + _name + " already exists");
		return nullptr;
	}

	// Get node to allocate from
	BufferNode* targetBuffer = bufferNodeHead;
	while (targetBuffer != nullptr)
	{
		if (targetBuffer->state == MEMORY_STATE::FREE)
		{
			if (targetBuffer->memoryInfo.size >= alignedSize)
				break;
			else
				targetBuffer = targetBuffer->next;
		}
		else
			targetBuffer = targetBuffer->next;
	}

	// If failed to find node to allocate from
	if (targetBuffer == nullptr)
	{
		Error::Report("Failed to allocate buffer, could not find empty space with fitting size");
		return nullptr;
	}

	// If node does NOT needs to be split
	if (targetBuffer->memoryInfo.size != alignedSize)
	{
		BufferNode* newBuffer = new BufferNode;
		*newBuffer = BufferNode::Get("Free", MemoryMap::MemoryInfo::Get(targetBuffer->memoryInfo.offset + alignedSize, targetBuffer->memoryInfo.size - alignedSize), MEMORY_STATE::FREE, targetBuffer, targetBuffer->next, nullptr);

		if (targetBuffer->next != nullptr)
			targetBuffer->next->prev = newBuffer;

		targetBuffer->memoryInfo.size -= newBuffer->memoryInfo.size;
		targetBuffer->next = newBuffer;
	}

	// Set pool values
	targetBuffer->name = _name;
	targetBuffer->state = MEMORY_STATE::GARBAGE;
	targetBuffer->userdata = _userdata;

	// Add pool size to map
	usedSpace += alignedSize;

	// Add pool to map
	bufferPtrs[_name] = targetBuffer;

	return targetBuffer;
}
MemoryMap::BufferNode* MemoryMap::PoolNode::GetBuffer(const char * _name)
{
	std::map<std::string, BufferNode*>::iterator iter = bufferPtrs.find(_name);
	if (iter == bufferPtrs.end())
		return nullptr;
	else
		return iter->second;
}
MemoryMap::Userdata MemoryMap::PoolNode::FreeBuffer(BufferNode * _buffer)
{
	if (_buffer == nullptr)
		Error::Report("Buffer is nullptr");

	BufferNode* prev = _buffer->prev;
	BufferNode* next = _buffer->next;
	Userdata userdata = _buffer->userdata;

	// Remove pool size from map
	usedSpace -= _buffer->memoryInfo.size;

	// Merge previous buffer
	if (prev != nullptr)
	{
		if (prev->state == MEMORY_STATE::FREE)
		{
			// Add memoryInfo
			_buffer->memoryInfo.offset -= prev->memoryInfo.size;
			_buffer->memoryInfo.size += prev->memoryInfo.size;

			// Point
			_buffer->prev = prev->prev;
			if (prev->prev != nullptr)
				prev->prev->next = _buffer;

			// move head
			if (prev == bufferNodeHead)
				bufferNodeHead = _buffer;

			delete prev;
		}
	}

	// Merge next buffer
	if (next != nullptr)
	{
		if (next->state == MEMORY_STATE::FREE)
		{
			// Add memoryInfo
			_buffer->memoryInfo.size += next->memoryInfo.size;

			// Point
			_buffer->next = next->next;
			if (next->next != nullptr)
				next->next->prev = _buffer;

			delete next;
		}
	}

	// Free buffer
	_buffer->name = "Free";
	_buffer->state = MEMORY_STATE::FREE;

	// Remove buffer from map
	bufferPtrs.erase(_buffer->name);

	return userdata;
}

