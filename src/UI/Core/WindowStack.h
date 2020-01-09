#pragma once

#include <list>

#include "Window.h"

class WindowStack final
{
public:
	WindowStack() = default;
	~WindowStack() = default;

	void addWindow(Window* _w);
	void removeWindow(Window* _w);

	bool pointInWindow(const NAS2D::Point_2d& _p) const { return pointInWindow(_p.x(), _p.y()); }
	bool pointInWindow(int x, int y) const;

	void updateStack(const NAS2D::Point_2d& _p) { updateStack(_p.x(), _p.y()); }
	void updateStack(int x, int y);

	void bringToFront(Window* _w);

	void hide();

	void update();

private:
	using WindowList = std::list<Window*>;

private:
	WindowList		mWindowList;
};
