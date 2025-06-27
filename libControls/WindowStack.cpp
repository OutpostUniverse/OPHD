#include "WindowStack.h"

#include "Window.h"

#include <algorithm>
#include <stdexcept>


void WindowStack::addWindow(Window& window)
{
	if (find(mWindowList.begin(), mWindowList.end(), &window) != mWindowList.end())
	{
		throw std::runtime_error("WindowStack::addWindow(): Attempting to add a Window that's already in this stack.");
	}

	mWindowList.push_back(&window);
}


void WindowStack::removeWindow(Window& window)
{
	mWindowList.remove(&window);
}


void WindowStack::bringToFront(Window& window)
{
	const auto windowPosition = find(mWindowList.begin(), mWindowList.end(), &window);
	if (windowPosition == mWindowList.end())
	{
		throw std::runtime_error("WindowStack::bringToFront(): Window is not managed by this stack.");
	}
	if (windowPosition == mWindowList.begin())
	{
		return;
	}

	mWindowList.front()->hasFocus(false);

	mWindowList.remove(&window);
	mWindowList.push_front(&window);
	window.hasFocus(true);
}


void WindowStack::updateStack(const NAS2D::Point<int>& point)
{
	for (auto* window : mWindowList)
	{
		if (window->visible() && window->area().contains(point))
		{
			bringToFront(*window);
			return;
		}
	}
}


bool WindowStack::pointInWindow(const NAS2D::Point<int>& point) const
{
	for (auto* window : mWindowList)
	{
		if (window->visible() && window->area().contains(point))
		{
			return true;
		}
	}

	return false;
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
