#pragma once

#include <Windows.h>
#include <vector>
#include <functional>
#include <map>
#include "MenuBar.h"
using namespace std;

class MenuItem
{
public:
	using Handler = function<void(MenuItem*)>;
	MenuItem(LPCWSTR text);
	MenuItem(LPCWSTR text, HBITMAP hBitmap);
	MenuItem(LPCWSTR text, initializer_list<MenuItem*> items, Handler onDropDownOpen = NULL);
	MenuItem(LPCWSTR text, HBITMAP hBitmap, initializer_list<MenuItem*> items, Handler onDropDownOpen = NULL);
	MenuItem(LPCWSTR text, Handler onClick);
	MenuItem(LPCWSTR text, HBITMAP hBitmap, Handler onClick);
	MenuItem(const MenuItem&) = delete;
	MenuItem& operator =(const MenuItem&) = delete;
	~MenuItem();
	LPCWSTR text;
	bool AddItem(MenuItem* item, int index = -1);
	bool AddItemRange(initializer_list<MenuItem*> items, int index = -1);
	bool RemoveItem(MenuItem* item, BOOL free = false);
	bool RemoveItemRange(int index = 0, int count = -1, BOOL free = false);
	void Clear(BOOL free = false);
	static MenuItem* Separator() { return new MenuItem(L"-"); }
private:
	friend class Window;
	friend class MenuBar;
	unsigned long long int id;
	MenuBar* menuBar;
	vector<MenuItem*> m_menuItems;
	HMENU m_hMenu;
	HBITMAP m_hBitmap;
	void UpdateMenu(function<void()> whenUpdate = NULL);
	Handler onDropDownOpen;
	Handler onClick;
	static vector<MenuItem*> menuItemMap;
	static MenuItem* GetMenuItemFromHandle(HMENU hMenu);
};
