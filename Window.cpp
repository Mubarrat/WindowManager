#include "Window.h"
#include <iostream>

Window::Window(LPCWSTR title)
	: m_hInstance(GetModuleHandle(nullptr)), m_hWnd(nullptr), menuBar(nullptr)
{
	this->title = title;
	size_t length = wcslen(title) + wcslen(L" Class");
	wcharArray = new wchar_t[length + 1];
	wcscpy_s(wcharArray, length + 1, title);
	wcscat_s(wcharArray, length + 1, L" Class");
	wprintf(L"%s\n", wcharArray);
	this->CLASS_NAME = wcharArray;

	WNDCLASS wndClass = {};
	if (!GetClassInfo(m_hInstance, CLASS_NAME, &wndClass))
	{
		wndClass.style = CS_HREDRAW | CS_VREDRAW;
		wndClass.lpfnWndProc = WindowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = 0;
		wndClass.hInstance = m_hInstance;
		wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
		wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
		wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wndClass.lpszClassName = CLASS_NAME;

		RegisterClass(&wndClass);
	}

	DWORD style = WS_OVERLAPPEDWINDOW;
	RECT rect = { x(), y(), x() + width(), y() + height()};
	AdjustWindowRect(&rect, style, false);

	m_hWnd = CreateWindowEx(
		0,
		CLASS_NAME,
		title,
		style,
		rect.left,
		rect.top,
		rect.right - rect.left,
		rect.bottom - rect.top,
		nullptr,
		nullptr,
		m_hInstance,
		nullptr
	);

	SetWindowLongPtr(m_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
}

Window::Window(LPCWSTR title, std::initializer_list<MenuItem*> items) : Window(title)
{
	this->menuBar = new MenuBar(m_hWnd, items);
}

Window::~Window()
{
	UnregisterClass(CLASS_NAME, m_hInstance);
	delete[] wcharArray;
}

bool Window::Initialize(HINSTANCE hInstance, int nCmdShow)
{
	if (m_hWnd)
	{
		ShowWindow(m_hWnd, nCmdShow);
		return true;
	}
	return false;
}

bool Window::ProcessMessages(LPMSG msg)
{
	while (PeekMessage(msg, nullptr, 0u, 0u, PM_REMOVE))
	{
		if (msg->message == WM_QUIT)
			return false;
		TranslateMessage(msg);
		DispatchMessage(msg);
	}
	return true;
}

int Window::x()
{
	return 250;
}

int Window::y()
{
	return 250;
}

LRESULT CALLBACK Window::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Window* window = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	return window ? window->WndProc(hWnd, uMsg, wParam, lParam) : DefWindowProc(hWnd, uMsg, wParam, lParam);
}

int Window::width()
{
	return 800;
}

int Window::height()
{
	return 600;
}

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_INITMENUPOPUP:
		{
			MenuItem* menuItem = MenuItem::GetMenuItemFromHandle(reinterpret_cast<HMENU>(wParam));
			if (menuItem && menuItem->onDropDownOpen)
				menuItem->onDropDownOpen(menuItem);
		}
		break;
	case WM_COMMAND:
		{
			MenuItem* menuItem = MenuItem::menuItemMap[LOWORD(wParam)];
			if (menuItem && menuItem->onClick)
				menuItem->onClick(menuItem);
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
