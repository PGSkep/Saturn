#include "Input2.h"

HWND Input2::hWnd = NULL;
HINSTANCE Input2::hInstance = NULL;
const char* Input2::name = "Input Window";
Input2::state Input2::data[dataSize];

void Input2::UpdateData(LPARAM _lParam)
{
	static char buffer[sizeof(RAWINPUT)] = {};
	static UINT size = sizeof(RAWINPUT);
	GetRawInputData((HRAWINPUT)_lParam, RID_INPUT, buffer, &size, sizeof(RAWINPUTHEADER));

	RAWINPUT* raw = (RAWINPUT*)buffer;
	HANDLE deviceHandle = raw->header.hDevice;
	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		const RAWKEYBOARD& rawKB = raw->data.keyboard;

		UINT flags = rawKB.Flags;
		UINT scanCode = rawKB.MakeCode;
		UINT virtualKey = rawKB.VKey;

		switch (virtualKey)
		{
		case 255:
			return;
		case VK_SHIFT:
			virtualKey = MapVirtualKey(scanCode, MAPVK_VSC_TO_VK_EX);
			break;
		case VK_NUMLOCK:
			scanCode = (MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC) | 0x100);
		}

		const bool isE0 = ((flags & RI_KEY_E0) != 0);
		const bool isE1 = ((flags & RI_KEY_E1) != 0);

		if (isE1)
		{
			if (virtualKey == VK_PAUSE)
				scanCode = 0x45;
			else
				scanCode = MapVirtualKey(virtualKey, MAPVK_VK_TO_VSC);
		}

		const bool isUp = ((flags & RI_KEY_BREAK) != 0);

		UINT res = (scanCode + isE0 * 56);
		if (res == 325)
			res = 86;

		keys key = (keys)res;

		if (isUp)
			data[key] = RELEASED;
		else
			data[key] = PRESSED;

		//UINT key2 = (scanCode << 16) | (isE0 << 24);
		//char buffer2[512] = {};
		//GetKeyNameText((LONG)key2, buffer2, 512);
		//
		//if (isUp)
		//	std::cout << buffer2 << " = " << res << ",\n";
	}
	else if (raw->header.dwType == RIM_TYPEMOUSE)
	{
		const RAWMOUSE& rawM = raw->data.mouse;
		USHORT flags = rawM.usButtonFlags;
		short wheelDelta = (short)rawM.usButtonData;
		LONG x = rawM.lLastX;
		LONG y = rawM.lLastY;

		*((int16_t*)&data[MOUSE_DELTA_X_S16]) += (int16_t)x;
		*((int16_t*)&data[MOUSE_DELTA_Y_S16]) += (int16_t)y;

		switch (flags)
		{
			// left
		case 1:
			data[MOUSE_LEFT] = state::PRESSED;
			break;
		case 2:
			data[MOUSE_LEFT] = state::RELEASED;
			break;
			// right
		case 4:
			data[MOUSE_RIGHT] = state::PRESSED;
			break;
		case 8:
			data[MOUSE_RIGHT] = state::RELEASED;
			break;
			// middle
		case 16:
			data[MOUSE_MID] = state::PRESSED;
			break;
		case 32:
			data[MOUSE_MID] = state::RELEASED;
			break;
			// wheel
		case 1024:
			if (wheelDelta > 0)
				++*((int16_t*)&data[MOUSE_WHEEL_DELTA_UP_U16]);
			else
				++*((int16_t*)&data[MOUSE_WHEEL_DELTA_DOWN_U16]);
			break;
		}

		//std::cout << "	Flags=" << flags << "	WheelDelta=" << wheelDelta << "	X=" << x << "	Y=" << y << "\n";
	}
}

LRESULT Input2::InputWndProc(HWND _hWnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam)
{
	switch (_uMsg)
	{
	case WM_INPUT:
		UpdateData(_lParam);
		break;
	}

	return DefWindowProc(_hWnd, _uMsg, _wParam, _lParam);
}

void Input2::Init(const char* _name)
{
	name = _name;
	hInstance = GetModuleHandle(NULL);

	WNDCLASSEX winClassEx;
	winClassEx.cbSize = sizeof(WNDCLASSEX);
	winClassEx.style = CS_HREDRAW | CS_VREDRAW;
	winClassEx.lpfnWndProc = InputWndProc;
	winClassEx.cbClsExtra = 0;
	winClassEx.cbWndExtra = 0;
	winClassEx.hInstance = hInstance;
	winClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	winClassEx.lpszMenuName = NULL;
	winClassEx.lpszClassName = _name;
	winClassEx.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

	if (!RegisterClassEx(&winClassEx))
		return;

	hWnd = CreateWindowEx(
		0,
		name,
		name,
		0,
		0, 0,
		0,
		0,
		NULL,
		NULL,
		hInstance,
		NULL);

	RAWINPUTDEVICE keyboardDevice;
	keyboardDevice.usUsagePage = 0x01;
	keyboardDevice.usUsage = 0x06;
	keyboardDevice.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
	keyboardDevice.hwndTarget = hWnd;
	RegisterRawInputDevices(&keyboardDevice, 1, sizeof(RAWINPUTDEVICE));

	RAWINPUTDEVICE mouseDevice;
	mouseDevice.usUsagePage = 0x01;
	mouseDevice.usUsage = 0x02;
	mouseDevice.dwFlags = RIDEV_NOLEGACY | RIDEV_INPUTSINK;
	mouseDevice.hwndTarget = hWnd;
	RegisterRawInputDevices(&mouseDevice, 1, sizeof(RAWINPUTDEVICE));

	if (hWnd == NULL)
	{
		UnregisterClass(name, hInstance);
		return;
	}
}

void Input2::Reset()
{
	memset(data, IDLE, sizeof(state) * dataSize);
}

void Input2::Update1()
{
	static const keys keyArr[]
	{
		ESC,	KEY_1,	KEY_2,	KEY_3,	KEY_4,	KEY_5,	KEY_6,	KEY_7,	KEY_8,	KEY_9,	KEY_0,	UNDERSCORE,	EQUAL,	BACKSPACE,	TAB,	Q,	W,	E,	R,	T,	Y,	U,	I,	O,	P,
		OPEN_BRACKET,	CLOSE_BRACKET,	ENTER,	CONTROL_LEFT,	A,	S,	D,	F,	G,	H,	J,	K,	L,	SEMICOLON,	QUOTE,	TILDE,	SHIFT_LEFT,	SLASH,	Z,	X,	C,	V,	B,	N,	M,
		COMMA,	PERIOD,	QUESTION_MARK,	SHIFT_RIGHT,	PAD_MULT,	ALT_LEFT,	SPACE,	CAPS,	F1,	F2,	F3,	F4,	F5,	F6,	F7,	F8,	F9,	F10,	PAUSE,	SCROLL_LOCK,	PAD_7,	PAD_8,
		PAD_9,	PAD_MINUS,	PAD_4,	PAD_5,	PAD_6,	PAD_PLUS,	PAD_1,	PAD_2,	PAD_3,	PAD_0,	PAD_DEL,	PAD_ENTER,	CONTROL_RIGHT,	NUM_LOCK,	F11,	F12,	MOUSE_LEFT,
		MOUSE_MID,	MOUSE_RIGHT,	PAD_DIVIDE,	PRINT_SCREEN,	ALT_RIGHT,	HOME,	ARROW_UP,	PAGE_UP,	ARROW_LEFT,	ARROW_RIGHT,	END,	ARROW_DOWN,	PAGE_DOWN,	INSERT,
		DEL,	OS_LEFT,	OS_RIGHT,	APP,
	};

	for (size_t i = 0; i != sizeof(keyArr) / sizeof(keys); ++i)
	{
		if (data[keyArr[i]] == state::PRESSED)
			data[keyArr[i]] = state::HELD;
		else if (data[keyArr[i]] == state::RELEASED)
			data[keyArr[i]] = state::IDLE;
	}

	static const keys keyArr2[] =
	{
		MOUSE_DELTA_X_S16,	MOUSE_DELTA_Y_S16,
		MOUSE_WHEEL_DELTA_UP_U16,	MOUSE_WHEEL_DELTA_DOWN_U16,
	};

	for (size_t i = 0; i != sizeof(keyArr2) / sizeof(keys); ++i)
	{
		*((uint16_t*)&data[keyArr2[i]]) = 0U;
	}
}
void Input2::Update2()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}
void Input2::Update3()
{
	POINT point;
	GetCursorPos(&point);

	*((int16_t*)&data[MOUSE_X_S16]) = (int16_t)point.x;
	*((int16_t*)&data[MOUSE_Y_S16]) = (int16_t)point.y;
}
void Input2::Update()
{
	Update1();
	Update2();
	Update3();
}

void Input2::ShutDown()
{
	DestroyWindow(hWnd);
	UnregisterClass(name, hInstance);
}
