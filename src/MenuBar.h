#pragma once

#include <Windows.h>
#include <vector>
#include <functional>
#include "MenuItem.h"
using namespace std;

class MenuItem;
class MenuBar
{
public:
	MenuBar(HWND hWnd, initializer_list<MenuItem*> items = {});
	MenuBar(const MenuBar&) = delete;
	MenuBar& operator =(const MenuBar&) = delete;
	~MenuBar();
	bool AddItem(MenuItem* item, int index = -1);
	bool AddItemRange(initializer_list<MenuItem*> items, int index = -1);
	bool RemoveItem(MenuItem* item);
	void Clear();
private:
	friend class Window;
	friend class MenuItem;
	HWND m_hWnd;
	vector<MenuItem*> m_menuItems;
	HMENU m_hMenu;
	void UpdateMenu(function<void()> whenUpdate = NULL);
};
