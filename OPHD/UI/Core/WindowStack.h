#pragma once

#include <list>

#include "Window.h"

class WindowStack final
{
public:
	WindowStack() = default;
	~WindowStack() = default;

	void addWindow(Window* window);
	void removeWindow(Window* window);

	bool pointInWindow(const NAS2D::Point<int>& p) const { return pointInWindow(p.x(), p.y()); }
	bool pointInWindow(int x, int y) const;

	void updateStack(const NAS2D::Point<int>& p) { updateStack(p.x(), p.y()); }
	void updateStack(int x, int y);

	void bringToFront(Window* window);

	void hide();

	virtual void update();

private:
	using WindowList = std::list<Window*>;

private:
	WindowList mWindowList;
};
