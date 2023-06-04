#include "Window.h"

#include "../../Cache.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


Window::Window(std::string newTitle) :
	mTitleFont{getDefaultFontBold()},
	mTitleBarLeft{imageCache.load("ui/skin/window_title_left.png")},
	mTitleBarCenter{imageCache.load("ui/skin/window_title_middle.png")},
	mTitleBarRight{imageCache.load("ui/skin/window_title_right.png")},
	mBody{
		imageCache.load("ui/skin/window_top_left.png"),
		imageCache.load("ui/skin/window_top_middle.png"),
		imageCache.load("ui/skin/window_top_right.png"),
		imageCache.load("ui/skin/window_middle_left.png"),
		imageCache.load("ui/skin/window_middle_middle.png"),
		imageCache.load("ui/skin/window_middle_right.png"),
		imageCache.load("ui/skin/window_bottom_left.png"),
		imageCache.load("ui/skin/window_bottom_middle.png"),
		imageCache.load("ui/skin/window_bottom_right.png")
	}
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


void Window::onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	UIContainer::onMouseDown(button, position);

	const auto titleBarBounds = NAS2D::Rectangle{mRect.position, {mRect.size.x, sWindowTitleBarHeight}};
	mMouseDrag = (button == NAS2D::EventHandler::MouseButton::Left && titleBarBounds.contains(position));
}


void Window::onMouseUp(NAS2D::EventHandler::MouseButton /*button*/, NAS2D::Point<int> /*position*/)
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


void Window::show()
{
	Control::show();
}


void Window::update()
{
	if (!visible()) { return; }

	draw();

	UIContainer::update();
}


void Window::draw() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	renderer.drawImage(mTitleBarLeft, mRect.position);
	renderer.drawImageRepeated(mTitleBarCenter, NAS2D::Rectangle<int>{{mRect.position.x + 4, mRect.position.y}, {mRect.size.x - 8, sWindowTitleBarHeight}});
	renderer.drawImage(mTitleBarRight, NAS2D::Point{mRect.position.x + mRect.size.x - 4, mRect.position.y});

	mBody.draw(renderer, NAS2D::Rectangle<int>{{mRect.position.x, mRect.position.y + 20}, {mRect.size.x, mRect.size.y - 20}});

	renderer.drawText(mTitleFont, title(), NAS2D::Point{mRect.position.x + 5, mRect.position.y + 2}, NAS2D::Color::White);
}


void Window::title(const std::string& title)
{
	mTitle = title;
	onTitleChanged();
}
