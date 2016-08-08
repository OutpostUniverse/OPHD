#pragma once

#include <list>

#include "Window.h"

class WindowStack
{
public:
	typedef std::list<Window*> WindowList;

public:

	WindowStack();
	~WindowStack();

	void addWindow(Window* _w);
	void removeWindow(Window* _w);

	bool pointInWindow(const Point_2d& _p) const { return pointInWindow(_p.x(), _p.y()); }
	bool pointInWindow(int x, int y) const;

	void updateStack(const Point_2d& _p) { updateStack(_p.x(), _p.y()); }
	void updateStack(int x, int y);

	void bringToFront(Window* _w);
	void sendToBack(Window* _w);

	void update();

protected:
	
private:
	WindowList		mWindowList;
};