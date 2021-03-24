#include "Window.h"

#include "../../Cache.h"
#include "../../Common.h"
#include "../../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


Window::Window(std::string newTitle) :
	mTitleFont{fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL)},
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

	Utility<EventHandler>::get().mouseButtonUp().connect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Window::onMouseMove);
}


Window::~Window()
{
	Utility<EventHandler>::get().mouseButtonUp().disconnect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Window::onMouseMove);
}


void Window::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!visible() || !hasFocus()) { return; }

	UIContainer::onMouseDown(button, x, y);

	const auto titleBarBounds = NAS2D::Rectangle{mRect.x, mRect.y, mRect.width, sWindowTitleBarHeight};
	mMouseDrag = (button == EventHandler::MouseButton::Left && titleBarBounds.contains(NAS2D::Point{x, y}));
}


void Window::onMouseUp(EventHandler::MouseButton /*button*/, int /*x*/, int /*y*/)
{
	mMouseDrag = false;
}


void Window::onMouseMove(int /*x*/, int /*y*/, int dX, int dY)
{
	if (!visible() || !hasFocus()) { return; }

	if (mMouseDrag && !mAnchored)
	{
		position(position() + NAS2D::Vector{dX, dY});
	}
}

void Window::anchored(bool isAnchored)
{
	mAnchored = isAnchored;
}


void Window::show()
{
	Control::show();
	hasFocus(true);
}


void Window::update()
{
	if (!visible()) { return; }

	auto& renderer = Utility<Renderer>::get();

	renderer.drawImage(mTitleBarLeft, mRect.startPoint());
	renderer.drawImageRepeated(mTitleBarCenter, NAS2D::Rectangle{mRect.x + 4, mRect.y, mRect.width - 8, sWindowTitleBarHeight});
	renderer.drawImage(mTitleBarRight, NAS2D::Point{mRect.x + mRect.width - 4, mRect.y});

	mBody.draw(renderer, NAS2D::Rectangle{mRect.x, mRect.y + 20, mRect.width, mRect.height - 20});

	renderer.drawText(mTitleFont, title(), NAS2D::Point{mRect.x + 5, mRect.y + 2}, NAS2D::Color::White);

	UIContainer::update();
}

void Window::title(const std::string& title)
{
	mTitle = title;
	onTitleChanged();
}
