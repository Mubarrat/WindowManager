#pragma comment(lib, "ComCtl32.lib")
#pragma comment(lib, "DwmApi.lib")
#pragma comment(lib, "uxtheme.lib")

#include "MainWindow.h"
#include <string>
#include <CommCtrl.h>
#include <dwmapi.h>

MainWindow::MainWindow()
	: Window(L"Window Manager", {
		new MenuItem(L"&File", {
			new MenuItem(L"&Add Window", {
				MenuItem::Separator()
			}, [this](MenuItem* mi) {
				mi->Clear(TRUE);
				for (const HWND& window : GetVisibleWindowsInfo()) {
					int textLength = GetWindowTextLength(window);
					auto title = new wchar_t[textLength + 1];
					int rl = GetWindowText(window, title, textLength + 1);
					if (rl != 0 && wcscmp(title, L"Program Manager") != 0 && wcscmp(title, L"Windows Input Experience") != 0)
						mi->AddItem(new MenuItem(title, GetAppIcon(window), [window, this](MenuItem* mi) {
							SetParent(window, m_mdiClient);
							SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) | WS_EX_MDICHILD);
							SetWindowSubclass(window, MdiChildProc, MdiChildProcId, 0);
							SetWindowPos(window, NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
						}));
				}
			}),
			new MenuItem(L"&Release Window", {
				MenuItem::Separator()
			}, [this](MenuItem* mi) {
				mi->Clear(TRUE);
				for (const HWND& window : GetMdiChildren()) {
					int textLength = GetWindowTextLength(window);
					auto title = new wchar_t[textLength + 1];
					int rl = GetWindowText(window, title, textLength + 1);
					if (rl != 0 && wcscmp(title, L"Program Manager") != 0 && wcscmp(title, L"Windows Input Experience") != 0)
						mi->AddItem(new MenuItem(title, GetAppIcon(window), [window, this](MenuItem* mi) {
							SetParent(window, 0);
							SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) & ~WS_EX_MDICHILD);
							RemoveWindowSubclass(window, MdiChildProc, MdiChildProcId);
						}));
				}
			}),
			MenuItem::Separator(),
			new MenuItem(L"E&xit", [this](MenuItem* mi) {
				SendMessage(m_hWnd, WM_CLOSE, 0, 0);
			})
		}),
		new MenuItem(L"Window", {
			new MenuItem(L"&Cascade Windows", [this](MenuItem* mi) {
				SendMessage(m_mdiClient, WM_MDICASCADE, 0, 0);
			}),
			new MenuItem(L"Tile &Horizontal", [this](MenuItem* mi) {
				SendMessage(m_mdiClient, WM_MDITILE, MDITILE_HORIZONTAL, 0);
			}),
			new MenuItem(L"Tile &Vertical", [this](MenuItem* mi) {
				SendMessage(m_mdiClient, WM_MDITILE, MDITILE_VERTICAL, 0);
			}),
			new MenuItem(L"&Arrange Icons", [this](MenuItem* mi) {
				SendMessage(m_mdiClient, WM_MDIICONARRANGE, 0, 0);
			})
		}, [this](MenuItem* mi) {
			mi->RemoveItemRange(4, -1, TRUE);
			auto items = GetMdiChildren();
			if (items.size() > 0)
				mi->AddItem(MenuItem::Separator());
			for (const HWND& window : items) {
				int textLength = GetWindowTextLength(window);
				auto title = new wchar_t[textLength + 1];
				int rl = GetWindowText(window, title, textLength + 1);
				if (rl != 0 && wcscmp(title, L"Program Manager") != 0 && wcscmp(title, L"Windows Input Experience") != 0)
					mi->AddItem(new MenuItem(title, GetAppIcon(window), [window, this](MenuItem* mi) {
						SetForegroundWindow(window);
					}));
			}
		}),
		new MenuItem(L"&Help", {
			new MenuItem(L"&About", [](MenuItem* mi) {
				MessageBox(NULL, L"Window Manager is made by Mubarrat Hasan. Visit https://mubarrat.com to get more softwares.", L"Window Manager", MB_ICONINFORMATION | MB_OK);
			})
		})
	})
{
	INITCOMMONCONTROLSEX iccx = {
		sizeof(INITCOMMONCONTROLSEX),
		ICC_WIN95_CLASSES
	};
	InitCommonControlsEx(&iccx);

	CLIENTCREATESTRUCT ccs{};
	m_mdiClient = CreateWindowEx(0, L"MDICLIENT", NULL, WS_CHILD | WS_CLIPCHILDREN | WS_VSCROLL | WS_HSCROLL |
		WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, m_hWnd, NULL,
		m_hInstance, (LPVOID)&ccs);
	if (!m_mdiClient)
	{
		MessageBox(NULL, L"Error creating MDI client.", L"Error", MB_ICONERROR | MB_OK);
	}
}

LRESULT CALLBACK MainWindow::MdiChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	switch (uMsg)
	{
	case WM_CHILDACTIVATE:
	case WM_GETMINMAXINFO:
	case WM_MENUCHAR:
	case WM_MOVE:
	case WM_SETFOCUS:
	case WM_SIZE:
	case WM_SYSCOMMAND:
		return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
	}
	return DefMDIChildProc(hWnd, uMsg, wParam, lParam);
}

std::vector<HWND> MainWindow::GetVisibleWindowsInfo()
{
    std::vector<HWND> windows;
    EnumWindows([](HWND hWnd, LPARAM lParam) -> BOOL {
        if (IsWindowVisible(hWnd) && GetWindowTextLength(hWnd) > 0)
            reinterpret_cast<std::vector<HWND>*>(lParam)->push_back(hWnd);
        return 1;
    }, reinterpret_cast<LPARAM>(&windows));
	auto it = std::find(windows.begin(), windows.end(), m_hWnd);
	if (it != windows.end())
		windows.erase(it);
    return windows;
}

HBITMAP MainWindow::GetAppIcon(HWND hwnd)
{
	HICON iconHandle = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL2, 0);
	if (iconHandle == nullptr)
		iconHandle = (HICON)SendMessage(hwnd, WM_GETICON, ICON_SMALL, 0);
	if (iconHandle == nullptr)
		iconHandle = (HICON)SendMessage(hwnd, WM_GETICON, ICON_BIG, 0);
	if (iconHandle == nullptr)
		iconHandle = (HICON)GetClassLongPtr(hwnd, GCLP_HICON);
	if (iconHandle == nullptr)
		iconHandle = (HICON)GetClassLongPtr(hwnd, GCLP_HICONSM);
	return CreateBitmapFromIcon(iconHandle);
}

HBITMAP MainWindow::CreateBitmapFromIcon(HICON hIcon)
{
	ICONINFO iconInfo;
	BITMAP bm;
	if (GetIconInfo(hIcon, &iconInfo))
	{
		GetObject(iconInfo.hbmColor, sizeof(BITMAP), &bm);
		int targetWidth = 16;
		int targetHeight = 16;
		HICON hResizedIcon = (HICON)CopyImage(hIcon, IMAGE_ICON, targetWidth, targetHeight, LR_COPYFROMRESOURCE);
		if (hResizedIcon == NULL)
			return nullptr;
		ICONINFO resizedIconInfo;
		if (!GetIconInfo(hResizedIcon, &resizedIconInfo))
		{
			DestroyIcon(hResizedIcon);
			return nullptr;
		}
		BITMAPINFOHEADER bi = {
			sizeof(BITMAPINFOHEADER),
			targetWidth,
			-targetHeight,
			1,
			bm.bmBitsPixel,
			BI_RGB
		};
		HDC hDC = GetDC(NULL);
		HDC hMemDC = CreateCompatibleDC(hDC);
		void* pBits;
		HBITMAP hBitmap = CreateDIBSection(hDC, reinterpret_cast<BITMAPINFO*>(&bi), DIB_RGB_COLORS, &pBits, NULL, 0);
		if (hBitmap != NULL)
		{
			HBITMAP hOldBitmap = static_cast<HBITMAP>(SelectObject(hMemDC, hBitmap));
			DrawIconEx(hMemDC, 0, 0, hResizedIcon, targetWidth, targetHeight, 0, NULL, DI_NORMAL);
			SelectObject(hMemDC, hOldBitmap);
			DeleteDC(hMemDC);
			ReleaseDC(NULL, hDC);
			DeleteObject(resizedIconInfo.hbmColor);
			DeleteObject(resizedIconInfo.hbmMask);
			DestroyIcon(hResizedIcon);
			return hBitmap;
		}
		DeleteObject(resizedIconInfo.hbmColor);
		DeleteObject(resizedIconInfo.hbmMask);
		DestroyIcon(hResizedIcon);
	}
	return nullptr;
}

LRESULT MainWindow::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			{
				auto items = GetMdiChildren();
				if (!items.empty()) {
					int result = MessageBox(hWnd, L"Do you want to close all these inner windows?", L"Window Manager", MB_YESNOCANCEL);
					switch (result) {
					case IDYES:
						// Just closing, do nothing
						break;
					case IDNO:
						for (HWND window : items) {
							SetParent(window, 0);
							SetWindowLong(window, GWL_EXSTYLE, GetWindowLong(window, GWL_EXSTYLE) & ~WS_EX_MDICHILD);
							RemoveWindowSubclass(window, MdiChildProc, MdiChildProcId);
						}
						break;
					case IDCANCEL:
						return 0;
					}
				}
			}
			return Window::WndProc(hWnd, uMsg, wParam, lParam);
		case WM_DESTROY:
		case WM_INITMENUPOPUP:
		case WM_COMMAND:
			return Window::WndProc(hWnd, uMsg, wParam, lParam);
		case WM_SIZING:
			{
				for (HWND window : GetMdiChildren()) {
					WINDOWPLACEMENT wp{};
					GetWindowPlacement(window, &wp);
					if (wp.showCmd == 3)
					{
						RECT mdiClient{};
						GetWindowRect(m_mdiClient, &mdiClient);
						int width = mdiClient.right - mdiClient.left;
						int height = mdiClient.bottom - mdiClient.top;
						RECT childWindow{};
						GetWindowRect(window, &childWindow);
						int offsetX = mdiClient.left - childWindow.left;
						int offsetY = mdiClient.top - childWindow.top;
						MoveWindow(window, -offsetX, -offsetY, width + offsetX * 2, height + offsetY * 2, true);
					}
				}
			}
			break;
	}
	return DefFrameProc(hWnd, m_mdiClient, uMsg, wParam, lParam);
}

std::vector<HWND> MainWindow::GetMdiChildren()
{
	std::vector<HWND> childWindows;
	for (HWND hWndChild = GetWindow(m_mdiClient, GW_CHILD);
		hWndChild != NULL;
		hWndChild = GetWindow(hWndChild, GW_HWNDNEXT))
		if (IsWindowVisible(hWndChild) && GetWindowTextLength(hWndChild) > 0)
			childWindows.push_back(hWndChild);
	return childWindows;
}
