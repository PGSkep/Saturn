#ifndef	COMMAND_MANAGER_H
#define COMMAND_MANAGER_H

#include <assert.h>
#include <stdint.h>

class CommandBuffer
{
public:
	struct Command
	{
		void* data;
		void(*func)(void*);

		static inline Command Get(void* _data, void(*_func)(void*))
		{
			Command command;
			command.data = _data;
			command.func = _func;
			return command;
		}
	};

private:
	size_t commandDataBufferSize;
	void* commandDataBuffer;
	size_t commandDataOffset;

	size_t commandBufferCount;
	Command* commandBuffer;
	size_t commandBufferOffset;

public:
	void Init(size_t _dataBufferSize, size_t _maxCommands)
	{
		commandDataBufferSize = _dataBufferSize;
		commandDataBuffer = new uint8_t[commandDataBufferSize];

		commandBufferCount = _maxCommands;
		commandBuffer = new Command[_maxCommands];
	}

	void* AllocateCommandData(size_t _size)
	{
		if (commandDataOffset + _size > commandDataBufferSize)
			assert("COMMAND DATA BUFFER OUT OF SPACE");

		void* data = (void*)((size_t)commandDataBuffer + commandDataOffset);
		commandDataOffset += _size;

		return data;
	}
	void AddCommand(Command _command)
	{
		if (commandBufferOffset >= commandBufferCount)
			assert("COMMAND BUFFER OUT OF SPACE");

		commandBuffer[commandBufferOffset] = _command;
		++commandBufferOffset;
	}

	void RunCommands()
	{
		for (size_t i = 0; i != commandBufferOffset; ++i)
		{
			commandBuffer[i].func(commandBuffer[i].data);
		}
	}
	void ResetCommands()
	{
		commandDataOffset = 0;
		commandBufferOffset = 0;
	}

	void ShutDown()
	{
		ResetCommands();
		commandDataBufferSize = 0;
		commandBufferCount = 0;
		delete[] commandDataBuffer;
		delete[] commandBuffer;
	}
};

#endif