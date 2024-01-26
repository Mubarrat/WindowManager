#pragma once

#include "Window.h"
#include "MenuBar.h"

class MainWindow : public Window
{
public:
	MainWindow();
private:
	std::vector<HWND> GetVisibleWindowsInfo();
	static HBITMAP GetAppIcon(HWND hWnd);
	static HBITMAP CreateBitmapFromIcon(HICON hIcon);
	HWND m_mdiClient;
	static LRESULT CALLBACK MdiChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	const long MdiChildProcId = 1001;
	std::vector<HWND> GetMdiChildren();
protected:
	LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};

