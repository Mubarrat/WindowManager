#include "MenuItem.h"
#include <atlbase.h>

unsigned long long int _i = 0;

std::vector<MenuItem*> MenuItem::menuItemMap;

MenuItem::MenuItem(LPCWSTR text) : m_hMenu(CreatePopupMenu()), text(text), id(_i++)
{
    menuItemMap.push_back(this);
}

MenuItem::MenuItem(LPCWSTR text, HBITMAP hBitmap) : m_hMenu(CreatePopupMenu()), text(text), m_hBitmap(hBitmap), id(_i++)
{
    menuItemMap.push_back(this);
}

MenuItem::MenuItem(LPCWSTR text, std::initializer_list<MenuItem*> items, Handler onDropDownOpen)
    : m_hMenu(CreatePopupMenu()), text(text), onDropDownOpen(onDropDownOpen), id(_i++)
{
    AddItemRange(items);
    menuItemMap.push_back(this);
}

MenuItem::MenuItem(LPCWSTR text, HBITMAP hBitmap, initializer_list<MenuItem*> items, Handler onDropDownOpen)
    : m_hMenu(CreatePopupMenu()), text(text), onDropDownOpen(onDropDownOpen), m_hBitmap(hBitmap), id(_i++)
{
    AddItemRange(items);
    menuItemMap.push_back(this);
}

MenuItem::MenuItem(LPCWSTR text, Handler onClick)
    : m_hMenu(CreatePopupMenu()), text(text), onClick(onClick), id(_i++)
{
    menuItemMap.push_back(this);
}

MenuItem::MenuItem(LPCWSTR text, HBITMAP hBitmap, Handler onClick)
    : m_hMenu(CreatePopupMenu()), text(text), onClick(onClick), m_hBitmap(hBitmap), id(_i++)
{
    menuItemMap.push_back(this);
}

MenuItem::~MenuItem()
{
	for (const auto& item : m_menuItems)
		delete item;
    DestroyMenu(m_hMenu);
}

bool MenuItem::AddItem(MenuItem* item, int index)
{
    if (index < -1 || index > static_cast<int>(m_menuItems.size()))
        return false;
    UpdateMenu([item, index, this]()
        {
            m_menuItems.insert(index == -1 ? m_menuItems.end() : m_menuItems.begin() + index, item);
        });
    return true;
}

bool MenuItem::AddItemRange(std::initializer_list<MenuItem*> items, int index)
{
    if (index < -1 || index > static_cast<int>(m_menuItems.size()))
        return false;
    UpdateMenu([items, index, this]()
        {
            m_menuItems.insert(index == -1 ? m_menuItems.end() : m_menuItems.begin() + index, items.begin(), items.end());
        });
    return true;
}

bool MenuItem::RemoveItem(MenuItem* item, BOOL free)
{
    auto it = std::find(m_menuItems.begin(), m_menuItems.end(), item);
    if (it != m_menuItems.end())
    {
        UpdateMenu([this, it, item, free]()
        {
            m_menuItems.erase(it);
            if (free)
                delete item;
        });
        return true;
    }
    return false;
}

bool MenuItem::RemoveItemRange(int index, int count, BOOL free)
{
    if (index < 0 || index >= m_menuItems.size())
        return false;
    if (count == -1)
        count = static_cast<int>(m_menuItems.size()) - index;
    if (count < 1 || index + count > m_menuItems.size())
        return false;
    UpdateMenu([this, index, count, free]()
{
        if (free)
            for (int i = index; i < index + count; ++i)
                delete m_menuItems[i];
        m_menuItems.erase(m_menuItems.begin() + index, m_menuItems.begin() + index + count);
    });
    return true;
}

void MenuItem::Clear(BOOL free)
{
    UpdateMenu([this, free]()
        {
            if (free)
                for (MenuItem* item : m_menuItems)
                    delete item;
            m_menuItems.clear();
        });
}

MenuItem* MenuItem::GetMenuItemFromHandle(HMENU hMenu)
{
    auto it = std::find_if(menuItemMap.begin(), menuItemMap.end(),
        [hMenu](const MenuItem* item) { return item->m_hMenu == hMenu; });
    return (it != menuItemMap.end()) ? *it : nullptr;
}

void MenuItem::UpdateMenu(std::function<void()> whenUpdate)
{
    for (int i = static_cast<int>(m_menuItems.size()) - 1; i >= 0; i--)
        RemoveMenu(m_hMenu, i, MF_BYPOSITION);
    if (whenUpdate)
        whenUpdate();
    for (const auto& item : m_menuItems)
    {
        if (wcscmp(item->text, L"-") == 0)
            AppendMenu(m_hMenu, MF_SEPARATOR, 0, 0);
        else if (item->m_menuItems.size() > 0)
            AppendMenu(m_hMenu, MF_POPUP | MF_STRING, reinterpret_cast<UINT_PTR>(item->m_hMenu), item->text);
        else
            AppendMenu(m_hMenu, MF_STRING, item->id, item->text);
        if (item->m_hBitmap)
            SetMenuItemBitmaps(m_hMenu, GetMenuItemCount(m_hMenu) - 1, MF_BYPOSITION, item->m_hBitmap, item->m_hBitmap);
        item->UpdateMenu();
    }
}
