#include "MenuBar.h"

MenuBar::MenuBar(HWND hWnd, std::initializer_list<MenuItem*> items) : m_hWnd(hWnd), m_hMenu(CreateMenu())
{
    AddItemRange(items);
    SetMenu(m_hWnd, m_hMenu);
}

MenuBar::~MenuBar()
{
    for (const auto& item : m_menuItems)
        delete item;
	DestroyMenu(m_hMenu);
}

bool MenuBar::AddItem(MenuItem* item, int index)
{
    if (index < -1 || index > static_cast<int>(m_menuItems.size()))
        return false;
    UpdateMenu([item, index, this]()
        {
            m_menuItems.insert(index == -1 ? m_menuItems.end() : m_menuItems.begin() + index, item);
        });
    return true;
}

bool MenuBar::AddItemRange(std::initializer_list<MenuItem*> items, int index)
{
    if (index < -1 || index > static_cast<int>(m_menuItems.size()))
        return false;
    UpdateMenu([items, index, this]()
        {
            m_menuItems.insert(index == -1 ? m_menuItems.end() : (m_menuItems.begin() + index), items.begin(), items.end());
        });
    return true;
}

bool MenuBar::RemoveItem(MenuItem* item)
{
    auto it = std::find(m_menuItems.begin(), m_menuItems.end(), item);
    if (it != m_menuItems.end())
    {
        UpdateMenu([this, it]() {
            m_menuItems.erase(it);
            });
        return true;
    }
    return false;
}

void MenuBar::Clear()
{
    UpdateMenu([this]() { m_menuItems.clear(); });
}

void MenuBar::UpdateMenu(std::function<void()> whenUpdate)
{
    for (int i = static_cast<int>(m_menuItems.size()) - 1; i >= 0; i--)
        RemoveMenu(m_hMenu, i, MF_BYPOSITION);
    if (whenUpdate)
        whenUpdate();
    for (const auto& item : m_menuItems)
    {
        item->menuBar = this;
        if (wcscmp(item->text, L"-") == 0)
            AppendMenu(m_hMenu, MF_SEPARATOR, 0, 0);
        else if (item->m_menuItems.size() > 0)
            AppendMenu(m_hMenu, MF_POPUP | MF_STRING, reinterpret_cast<UINT_PTR>(item->m_hMenu), item->text);
        else
            AppendMenu(m_hMenu, MF_STRING, reinterpret_cast<UINT_PTR>(item), item->text);
        if (item->m_hBitmap)
            SetMenuItemBitmaps(m_hMenu, GetMenuItemCount(m_hMenu) - 1, MF_BYPOSITION, item->m_hBitmap, item->m_hBitmap);
        item->UpdateMenu();
    }
}
