#include "WindowStack.h"

#include "Window.h"
#include "../../Common.h"

#include <algorithm>
#include <stdexcept>


/**
 * Adds a Window to be handled by the WindowStack.
 *
 * \note	Pointer is not owned by WindowStack, it is up to the caller to properly handle memory.
 */
void WindowStack::addWindow(Window* window)
{
	if (find(mWindowList.begin(), mWindowList.end(), window) != mWindowList.end())
	{
		throw std::runtime_error("WindowStack::addWindow(): Attempting to add a Window that's already in this stack.");
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
bool WindowStack::pointInWindow(const NAS2D::Point<int>& point) const
{
	for (auto* window : mWindowList)
	{
		if (window->visible() && window->rect().contains(point))
		{
			return true;
		}
	}

	return false;
}


void WindowStack::updateStack(const NAS2D::Point<int>& point)
{
	for (auto* window : mWindowList)
	{
		if (window->visible() && window->rect().contains(point))
		{
			bringToFront(window);
			return;
		}
	}
}


void WindowStack::bringToFront(Window* window)
{
	const auto windowPosition = find(mWindowList.begin(), mWindowList.end(), window);
	if (windowPosition == mWindowList.end())
	{
		throw std::runtime_error("WindowStack::bringToFront(): Window is not managed by this stack.");
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
