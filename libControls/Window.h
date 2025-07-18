#pragma once

#include "ControlContainer.h"

#include <NAS2D/Renderer/RectangleSkin.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>

#include <string>


namespace NAS2D
{
	class Font;
	class Image;
}


class Window : public ControlContainer
{
public:
	Window(std::string newTitle = {}, const NAS2D::Font& titleFont = getDefaultFontBold());
	~Window() override;

	void anchored(bool isAnchored);

	void update() override;

	void title(const std::string& title);
	const std::string& title() const { return mTitle; }

protected:
	void draw(NAS2D::Renderer& renderer) const override;
	void drawTitleBar(NAS2D::Renderer& renderer) const;
	virtual void drawClientArea(NAS2D::Renderer& renderer) const;

	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position) override;
	void onMouseUp(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	static constexpr int sWindowTitleBarHeight = 20;

private:
	const NAS2D::Font& mTitleFont;
	const NAS2D::Image& mTitleBarLeft;
	const NAS2D::Image& mTitleBarCenter;
	const NAS2D::Image& mTitleBarRight;
	NAS2D::RectangleSkin mBody;

	std::string mTitle;

	bool mMouseDrag = false;
	bool mAnchored = false;
};
