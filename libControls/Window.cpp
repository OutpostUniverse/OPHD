#include "Window.h"

#include "LoadRectangleSkin.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>


Window::Window(std::string newTitle, const NAS2D::Font& titleFont) :
	mTitleFont{titleFont},
	mTitleBarLeft{getImage("ui/skin/window_title_left.png")},
	mTitleBarCenter{getImage("ui/skin/window_title_middle.png")},
	mTitleBarRight{getImage("ui/skin/window_title_right.png")},
	mBody{loadRectangleSkin("ui/skin/window")}
{
	title(newTitle);

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonUp().connect({this, &Window::onMouseUp});
	eventHandler.mouseMotion().connect({this, &Window::onMouseMove});
}


Window::~Window()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonUp().disconnect({this, &Window::onMouseUp});
	eventHandler.mouseMotion().disconnect({this, &Window::onMouseMove});
}


void Window::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	ControlContainer::onMouseDown(button, position);

	const auto titleBarBounds = NAS2D::Rectangle{mRect.position, {mRect.size.x, sWindowTitleBarHeight}};
	mMouseDrag = (button == NAS2D::MouseButton::Left && titleBarBounds.contains(position));
}


void Window::onMouseUp(NAS2D::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
{
	mMouseDrag = false;
}


void Window::onMouseMove(NAS2D::Point<int> /*position*/, NAS2D::Vector<int> relative)
{
	if (!enabled() || !visible()) { return; }

	if (mMouseDrag && !mAnchored)
	{
		position(position() + relative);
	}
}

void Window::anchored(bool isAnchored)
{
	mAnchored = isAnchored;
}


void Window::update()
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);

	ControlContainer::update();
}


void Window::draw(NAS2D::Renderer& renderer) const
{
	if (!visible()) { return; }

	drawTitleBar(renderer);
	drawClientArea();
}


void Window::drawTitleBar(NAS2D::Renderer& renderer) const
{
	renderer.drawImage(mTitleBarLeft, mRect.position);
	renderer.drawImageRepeated(mTitleBarCenter, NAS2D::Rectangle<int>{{mRect.position.x + 4, mRect.position.y}, {mRect.size.x - 8, sWindowTitleBarHeight}});
	renderer.drawImage(mTitleBarRight, NAS2D::Point{mRect.position.x + mRect.size.x - 4, mRect.position.y});

	mBody.draw(renderer, NAS2D::Rectangle<int>{{mRect.position.x, mRect.position.y + sWindowTitleBarHeight}, {mRect.size.x, mRect.size.y - sWindowTitleBarHeight}});

	renderer.drawText(mTitleFont, title(), NAS2D::Point{mRect.position.x + 5, mRect.position.y + 2}, NAS2D::Color::White);
}


void Window::drawClientArea() const
{
}


void Window::title(const std::string& title)
{
	mTitle = title;
}
