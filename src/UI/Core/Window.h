#pragma once

#include "UIContainer.h"

#include "NAS2D/Resources/Image.h"

class Window : public UIContainer
{
public:
	Window();
	virtual ~Window();

	void anchored(bool _a);

	virtual void show();
	virtual void update();

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseMotion(int x, int y, int dX, int dY);

	static inline constexpr float sWindowTitleBarHeight = 20.0f;

private:
	void _init();

private:
	bool				mMouseDrag = false;
	bool				mAnchored = false;

	NAS2D::ImageList	mTitle;
	NAS2D::ImageList	mBody;
};
