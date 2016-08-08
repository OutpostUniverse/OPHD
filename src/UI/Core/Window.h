#pragma once

#include "UIContainer.h"


class Window : public UIContainer
{
public:
	Window();
	virtual ~Window();


	virtual void update();

protected:

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseUp(MouseButton button, int x, int y);
	void onMouseMotion(int x, int y, int dX, int dY);

private:

private:
	Font		mBold;

	bool		mMouseDrag;
};