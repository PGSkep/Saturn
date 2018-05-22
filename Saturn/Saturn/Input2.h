#ifndef INPUT2_H
#define INPUT2_H

#include <Windows.h>
#include <vector>
#include <iostream>
#include <string>

class Input2
{
public:
	enum keys
	{
		ESC = 1,
		KEY_1 = 2,
		KEY_2 = 3,
		KEY_3 = 4,
		KEY_4 = 5,
		KEY_5 = 6,
		KEY_6 = 7,
		KEY_7 = 8,
		KEY_8 = 9,
		KEY_9 = 10,
		KEY_0 = 11,
		UNDERSCORE = 12,
		EQUAL = 13,
		BACKSPACE = 14,
		TAB = 15,
		Q = 16,
		W = 17,
		E = 18,
		R = 19,
		T = 20,
		Y = 21,
		U = 22,
		I = 23,
		O = 24,
		P = 25,
		OPEN_BRACKET = 26,
		CLOSE_BRACKET = 27,
		ENTER = 28,
		CONTROL_LEFT = 29,
		A = 30,
		S = 31,
		D = 32,
		F = 33,
		G = 34,
		H = 35,
		J = 36,
		K = 37,
		L = 38,
		SEMICOLON = 39,
		QUOTE = 40,
		TILDE = 41,
		SHIFT_LEFT = 42,
		SLASH = 43,
		Z = 44,
		X = 45,
		C = 46,
		V = 47,
		B = 48,
		N = 49,
		M = 50,
		COMMA = 51,
		PERIOD = 52,
		QUESTION_MARK = 53,
		SHIFT_RIGHT = 54,
		PAD_MULT = 55,
		ALT_LEFT = 56,
		SPACE = 57,
		CAPS = 58,
		F1 = 59,
		F2 = 60,
		F3 = 61,
		F4 = 62,
		F5 = 63,
		F6 = 64,
		F7 = 65,
		F8 = 66,
		F9 = 67,
		F10 = 68,
		PAUSE = 69,
		SCROLL_LOCK = 70,
		PAD_7 = 71,
		PAD_8 = 72,
		PAD_9 = 73,
		PAD_MINUS = 74,
		PAD_4 = 75,
		PAD_5 = 76,
		PAD_6 = 77,
		PAD_PLUS = 78,
		PAD_1 = 79,
		PAD_2 = 80,
		PAD_3 = 81,
		PAD_0 = 82,
		PAD_DEL = 83,
		PAD_ENTER = 84,
		CONTROL_RIGHT = 85,
		NUM_LOCK = 86,
		F11 = 87,
		F12 = 88,
		MOUSE_LEFT = 89, // mouse
		MOUSE_MID = 90, // mouse
		MOUSE_RIGHT = 91, // mouse
		MOUSE_DELTA_X_S16 = 92, // 93 mouse
		MOUSE_DELTA_Y_S16 = 94, // 95 mouse
		MOUSE_WHEEL_DELTA_UP_U16 = 96, // 97 mouse
		MOUSE_WHEEL_DELTA_DOWN_U16 = 98, // 99 mouse
		MOUSE_X_S16 = 100,// 101 mouse
		MOUSE_Y_S16 = 102,// 103 mouse

		PAD_DIVIDE = 109,

		PRINT_SCREEN = 111,
		ALT_RIGHT = 112,

		HOME = 127,
		ARROW_UP = 128,
		PAGE_UP = 129,

		ARROW_LEFT = 131,

		ARROW_RIGHT = 133,

		END = 135,
		ARROW_DOWN = 136,
		PAGE_DOWN = 137,
		INSERT = 138,
		DEL = 139,

		OS_LEFT = 147,
		OS_RIGHT = 148,
		APP = 149,
	};

	enum state : uint8_t
	{
		PRESSED = 8,
		HELD = 12,
		RELEASED = 2,
		IDLE = 3,
	};

private:
	static HWND hWnd;
	static HINSTANCE hInstance;
	static const char* name;
	static const uint8_t dataSize = 149;
	static state data[dataSize];

public:
	static void UpdateData(LPARAM _lParam);
	static LRESULT WINAPI InputWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam);
	static void Init(const char* _name);
	static void Reset();

	static void Update1();
	static void Update2();
	static void Update3();
	static void Update();

	// All keys
	static inline state GetState(keys _key)
	{
		return data[_key];
	}

	static inline bool IsUp(keys _key)
	{
		return (data[_key] & IDLE) != 0;
	}
	static inline bool IsDown(keys _key)
	{
		return (data[_key] & HELD) != 0;
	}
	static inline bool IsPressed(keys _key)
	{
		return data[_key] == PRESSED;
	}
	static inline bool IsReleased(keys _key)
	{
		return data[_key] == RELEASED;
	}
	static inline bool IsIdle(keys _key)
	{
		return data[_key] == IDLE;
	}
	static inline bool IsHeld(keys _key)
	{
		return data[_key] == HELD;
	}

	// MOUSE_X_S16, MOUSE_Y_S16, MOUSE_DELTA_X_S16, MOUSE_DELTA_Y_S16
	static inline int16_t GetMouse(keys _key)
	{
		return *((int16_t*)&data[_key]);
	}

	// MOUSE_WHEEL_DELTA_UP_U16, MOUSE_WHEEL_DELTA_DOWN_U16
	static inline uint16_t GetWheel(keys _key)
	{
		return *((uint16_t*)&data[_key]);
	}

	static void ShutDown();
};

#endif