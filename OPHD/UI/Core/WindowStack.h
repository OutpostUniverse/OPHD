#pragma once

#include "Window.h"

#include <list>


class WindowStack final
{
public:
	WindowStack() = default;
	~WindowStack() = default;

	void addWindow(Window* window);
	void removeWindow(Window* window);

	bool pointInWindow(const NAS2D::Point<int>& point) const;
	bool pointInWindow(int x, int y) const { return pointInWindow({x, y}); }

	void updateStack(const NAS2D::Point<int>& point);
	void updateStack(int x, int y) { updateStack({x, y}); }

	void bringToFront(Window* window);

	void hide();

	virtual void update();

private:
	using WindowList = std::list<Window*>;

private:
	WindowList mWindowList;
};
