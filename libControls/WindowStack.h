#pragma once

#include <NAS2D/Math/Point.h>

#include <list>


class Window;


class WindowStack
{
public:
	WindowStack() = default;
	~WindowStack() = default;

	void addWindow(Window& window);
	void removeWindow(Window& window);
	void bringToFront(Window& window);
	void updateStack(const NAS2D::Point<int>& point);
	bool pointInWindow(const NAS2D::Point<int>& point) const;

	void hide();

	void update();

private:
	std::list<Window*> mWindowList;
};
