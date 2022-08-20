#pragma once

#include "UIContainer.h"

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>
#include <NAS2D/Renderer/RectangleSkin.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>

#include <string>


class Window : public UIContainer
{
public:
	Window(std::string newTitle = "");
	~Window() override;

	void anchored(bool isAnchored);

	void show() override;

	void update() override;
	void draw() const override;

	using TitleChangeSignal = NAS2D::Signal<Window*>;

	void title(const std::string& title);
	const std::string& title() const { return mTitle; }
	TitleChangeSignal::Source& titleChanged() { return mTitleChanged; }

	virtual void onTitleChanged() { mTitleChanged(this); }

protected:
	void onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position) override;
	void onMouseUp(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	static constexpr int sWindowTitleBarHeight = 20;

private:
	const NAS2D::Font& mTitleFont;
	bool mMouseDrag = false;
	bool mAnchored = false;

	const NAS2D::Image& mTitleBarLeft;
	const NAS2D::Image& mTitleBarCenter;
	const NAS2D::Image& mTitleBarRight;
	NAS2D::RectangleSkin mBody;

	TitleChangeSignal mTitleChanged;

	std::string mTitle;
};
