#pragma once

#include "UIContainer.h"


class Window : public UIContainer
{
public:
	Window();
	virtual ~Window();

	void anchored(bool _a);

	virtual void show();
	virtual void update();

protected:
	void onMouseDown(EventHandler::MouseButton button, int x, int y);
	void onMouseUp(EventHandler::MouseButton button, int x, int y);
	void onMouseMotion(int x, int y, int dX, int dY);

private:
	void _init();

private:
	bool		mMouseDrag = false;
	bool		mAnchored = false;

	ImageList	mTitle;
	ImageList	mBody;
};