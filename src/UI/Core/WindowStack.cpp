#include "WindowStack.h"

WindowStack::WindowStack()
{}


WindowStack::~WindowStack()
{}


/**
 * Adds a Window to be handled by the WindowStack.
 * 
 * \note	Pointer is not owned by WindowStack, it is up to the caller to properly handle memory.
 */
void WindowStack::addWindow(Window* _w)
{
	if (find(mWindowList.begin(), mWindowList.end(), _w) != mWindowList.end())
	{
		cout << "WindowStack::addWindow(): Attempting to add a Window that's already in this stack." << endl;
		return;
	}

	mWindowList.push_back(_w);
}


/**
 * Removes a Window from the WindowStack.
 * 
 * \note Pointer is not owned by WindowStack, it is up to the caller to properly handle memory.
 */
void WindowStack::removeWindow(Window* _w)
{
	mWindowList.remove(_w);
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
		if (w->visible() && isPointInRect(x, y, w->rect().x(), w->rect().y(), w->rect().w(), w->rect().h()))
			return true;
	}

	return false;
}


/**
 * fixme	This function rolls together updating the stack during a click and returning whether or
 *			not said point is within the bounds of a window. Not sure if these two functions can be
 *			combined or if they should remain seperate?
 */
void WindowStack::updateStack(int x, int y)
{
	for (auto it = mWindowList.begin(); it != mWindowList.end(); ++it)
	{
		Window* w = (*it);
		if (w->visible() && isPointInRect(x, y, w->rect().x(), w->rect().y(), w->rect().w(), w->rect().h()))
		{
			if (it == mWindowList.begin())
				return;
			
			bringToFront(w);
			return;
		}
	}
}


void WindowStack::bringToFront(Window* _w)
{
	if (find(mWindowList.begin(), mWindowList.end(), _w) == mWindowList.end())
	{
		cout << "WindowStack::bringToFront(): Window is not managed by this stack." << endl;
		return;
	}

	(*mWindowList.begin())->hasFocus(false);

	mWindowList.remove(_w);
	mWindowList.push_front(_w);
	_w->hasFocus(true);
}


void WindowStack::sendToBack(Window* _w)
{
	if (find(mWindowList.begin(), mWindowList.end(), _w) == mWindowList.end())
	{
		cout << "WindowStack::bringToFront(): Window is not managed by this stack." << endl;
		return;
	}

	mWindowList.remove(_w);
	mWindowList.push_back(_w);
	_w->hasFocus(false);
	(*mWindowList.begin())->hasFocus(true);
}


void WindowStack::update()
{
	for (auto it = mWindowList.rbegin(); it != mWindowList.rend(); ++it)
		(*it)->update();
}
