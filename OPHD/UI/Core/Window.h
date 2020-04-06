#pragma once

#include "UIContainer.h"

#include "NAS2D/Resources/Image.h"

#include <string>


class Window : public UIContainer
{
public:
	Window(std::string newTitle = "");
	~Window() override;

	void anchored(bool isAnchored);

	void show() override;
	void update() override;

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y) override;
	void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int dX, int dY);

	static inline constexpr float sWindowTitleBarHeight = 20.0f;

private:
	void _init();

private:
	bool				mMouseDrag = false;
	bool				mAnchored = false;

	NAS2D::ImageList	mTitle;
	NAS2D::ImageList	mBody;
};
