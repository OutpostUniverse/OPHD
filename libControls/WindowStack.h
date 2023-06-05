#pragma once

#include <NAS2D/Math/Point.h>

#include <list>


class Window;


class WindowStack final
{
public:
	WindowStack() = default;
	~WindowStack() = default;

	void addWindow(Window* window);
	void removeWindow(Window* window);

	bool pointInWindow(const NAS2D::Point<int>& point) const;

	void updateStack(const NAS2D::Point<int>& point);

	void bringToFront(Window* window);

	void hide();

	void update();

private:
	using WindowList = std::list<Window*>;

	WindowList mWindowList;
};
