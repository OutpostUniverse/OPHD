#pragma once

#include "UIContainer.h"


class Window : public UIContainer
{
public:
	Window();
	virtual ~Window();


	virtual void update();

	void anchored(bool _a);

protected:

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseUp(MouseButton button, int x, int y);
	void onMouseMotion(int x, int y, int dX, int dY);

private:

private:
	Font		mBold;

	bool		mMouseDrag;
	bool		mAnchored;

	ImageList	mTitle;
	ImageList	mBody;
};