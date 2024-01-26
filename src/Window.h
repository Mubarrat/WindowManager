#pragma once

#include <Windows.h>
#include "MenuBar.h"

class Window
{
public:
	Window(LPCWSTR title);
	Window(LPCWSTR title, std::initializer_list<MenuItem*> items);
	Window(const Window&) = delete;
	Window& operator =(const Window&) = delete;
	~Window();
	bool Initialize(HINSTANCE hInstance, int nCmdShow);
	bool ProcessMessages(LPMSG msg);
	LPCWSTR title;
	MenuBar* menuBar;
private:
	const wchar_t* CLASS_NAME;
	wchar_t* wcharArray;
	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	virtual int x();
	virtual int y();
	virtual int width();
	virtual int height();
	HINSTANCE m_hInstance;
	HWND m_hWnd;
	virtual LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
