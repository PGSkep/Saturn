#ifndef	SURFACE_H
#define SURFACE_H

#include "VulkanCommon.h"

#include <Windows.h>
#include <string>

static HINSTANCE	hInstance;

class Surface
{
	std::string			name;
	HWND				hWnd;
	VkSurfaceKHR		surface;

public:
	static void SetHInstance()
	{
		hInstance = GetModuleHandle(NULL);
	}

	void Init(const char* _name, const char* _title, int _width, int _height, WNDPROC _wndProc, VkInstance _instance)
	{
		name = _name;
		hInstance = GetModuleHandle(NULL);

		WNDCLASSEX winClassEx;
		winClassEx.cbSize = sizeof(WNDCLASSEX);
		winClassEx.style = CS_HREDRAW | CS_VREDRAW;
		winClassEx.lpfnWndProc = _wndProc;
		winClassEx.cbClsExtra = 0;
		winClassEx.cbWndExtra = 0;
		winClassEx.hInstance = hInstance;
		winClassEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		winClassEx.hCursor = LoadCursor(NULL, IDC_ARROW);
		winClassEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		winClassEx.lpszMenuName = NULL;
		winClassEx.lpszClassName = name.c_str();
		winClassEx.hIconSm = LoadIcon(NULL, IDI_WINLOGO);

		if (!RegisterClassEx(&winClassEx))
			Error::Report("!RegisterClassEx");

		RECT rect = { 0, 0, _width, _height };
		DWORD dwStyle = WS_OVERLAPPEDWINDOW;
		AdjustWindowRect(&rect, dwStyle, FALSE);

		hWnd = CreateWindowEx(0,
			name.c_str(),
			_title,
			dwStyle | WS_VISIBLE | WS_SYSMENU,
			0, 0,
			rect.right - rect.left,
			rect.bottom - rect.top,
			NULL,
			NULL,
			hInstance,
			NULL);

		if (hWnd == NULL)
		{
			UnregisterClass(name.c_str(), hInstance);
			Error::Report("Failed to create a window");
		}

		ShowWindow(hWnd, SW_SHOW);
		SetForegroundWindow(hWnd);
		SetFocus(hWnd);

		VkWin32SurfaceCreateInfoKHR win32SurfaceCreateInfoKHR;
		win32SurfaceCreateInfoKHR.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		win32SurfaceCreateInfoKHR.pNext = nullptr;
		win32SurfaceCreateInfoKHR.flags = VK_RESERVED_FOR_FUTURE_USE;
		win32SurfaceCreateInfoKHR.hinstance = hInstance;
		win32SurfaceCreateInfoKHR.hwnd = hWnd;
		VulkanCommon::CreateWin32SurfaceKHR(_instance, &win32SurfaceCreateInfoKHR, surface);
	}
	void ShutDown(VkInstance _instance)
	{
		vkDestroySurfaceKHR(_instance, surface, nullptr);

		UnregisterClass(name.c_str(), hInstance);
		DestroyWindow(hWnd);
	}

	std::string GetName()
	{
		return name;
	}
	VkSurfaceKHR GetHandle()
	{
		return surface;
	}
};

#endif