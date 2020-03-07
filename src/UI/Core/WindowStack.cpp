// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "WindowStack.h"

#include "../../Common.h"

#include <algorithm>
#include <iostream>



/**
 * Adds a Window to be handled by the WindowStack.
 *
 * \note	Pointer is not owned by WindowStack, it is up to the caller to properly handle memory.
 */
void WindowStack::addWindow(Window* window)
{
	if (find(mWindowList.begin(), mWindowList.end(), window) != mWindowList.end())
	{
		std::cout << "WindowStack::addWindow(): Attempting to add a Window that's already in this stack." << std::endl;
		return;
	}

	mWindowList.push_back(window);
}


/**
 * Removes a Window from the WindowStack.
 *
 * \note Pointer is not owned by WindowStack, it is up to the caller to properly handle memory.
 */
void WindowStack::removeWindow(Window* window)
{
	mWindowList.remove(window);
}


/**
 * \fixme	Can this and updateStack() just be rolled together? I don't see a use case where
 *			this would be used unless we're pressing a mouse button except maybe for highlights
 *			during mouse moves?
 */
bool WindowStack::pointInWindow(int x, int y) const
{
	for (auto it = mWindowList.begin(); it != mWindowList.end(); ++it)
	{
		Window* w = *(it);
		if (w->visible() && w->rect().to<int>().contains(NAS2D::Point{x, y}))
		{
			return true;
		}
	}

	return false;
}


/**
 *
 */
void WindowStack::updateStack(int x, int y)
{
	for (auto it = mWindowList.begin(); it != mWindowList.end(); ++it)
	{
		Window* w = (*it);
		if (w->visible() && w->rect().to<int>().contains(NAS2D::Point{x, y}))
		{
			if (it == mWindowList.begin()) { return; }
			bringToFront(w);
			return;
		}
	}
}


void WindowStack::bringToFront(Window* window)
{
	const auto windowPosition = find(mWindowList.begin(), mWindowList.end(), window);
	if (windowPosition == mWindowList.end())
	{
		std::cout << "WindowStack::bringToFront(): Window is not managed by this stack." << std::endl;
		return;
	}
	if (windowPosition == mWindowList.begin())
	{
		return;
	}

	mWindowList.front()->hasFocus(false);

	mWindowList.remove(window);
	mWindowList.push_front(window);
	window->hasFocus(true);
}


void WindowStack::hide()
{
	for (auto it = mWindowList.rbegin(); it != mWindowList.rend(); ++it)
	{
		(*it)->hide();
	}
}


void WindowStack::update()
{
	for (auto it = mWindowList.rbegin(); it != mWindowList.rend(); ++it)
	{
		(*it)->update();
	}
}
