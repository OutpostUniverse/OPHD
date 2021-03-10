#pragma once

#include "UIContainer.h"

#include <NAS2D/Resources/Font.h>
#include <NAS2D/Resources/Image.h>
#include <NAS2D/Renderer/RectangleSkin.h>

#include <string>


class Window : public UIContainer
{
public:
	Window(std::string newTitle = "");
	~Window() override;

	void anchored(bool isAnchored);

	void show() override;
	void update() override;

	using TitleChangedCallback = NAS2D::Signals::Signal<Window*>;

	void title(const std::string& title);
	const std::string& title() const { return mTitle; }
	TitleChangedCallback& titleChanged() { return mTitleChanged; }

	virtual void onTitleChanged() { mTitleChanged(this); }

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y) override;
	void onMouseUp(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int dX, int dY);

	static constexpr int sWindowTitleBarHeight = 20;

private:
	const NAS2D::Font& mTitleFont;
	bool mMouseDrag = false;
	bool mAnchored = false;

	const NAS2D::Image& mTitleBarLeft;
	const NAS2D::Image& mTitleBarCenter;
	const NAS2D::Image& mTitleBarRight;
	NAS2D::RectangleSkin mBody;

	TitleChangedCallback mTitleChanged;

	std::string mTitle;
};
