// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Window.h"

#include "../../Common.h"
#include "../../Constants.h"
#include "../../FontManager.h"

#include "NAS2D/Utility.h"
#include "NAS2D/Renderer/Renderer.h"

using namespace NAS2D;

const float WINDOW_TITLE_BAR_HEIGHT = 20.0f;
static Font* WINDOW_TITLE_FONT;

Window::Window()
{
	_init();
}


Window::~Window()
{
	Utility<EventHandler>::get().mouseButtonUp().disconnect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Window::onMouseMotion);
}


void Window::_init()
{
	Utility<EventHandler>::get().mouseButtonUp().connect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Window::onMouseMotion);

	WINDOW_TITLE_FONT = Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);

	mBody.push_back(Image("ui/skin/window_top_left.png"));
	mBody.push_back(Image("ui/skin/window_top_middle.png"));
	mBody.push_back(Image("ui/skin/window_top_right.png"));
	mBody.push_back(Image("ui/skin/window_middle_left.png"));
	mBody.push_back(Image("ui/skin/window_middle_middle.png"));
	mBody.push_back(Image("ui/skin/window_middle_right.png"));
	mBody.push_back(Image("ui/skin/window_bottom_left.png"));
	mBody.push_back(Image("ui/skin/window_bottom_middle.png"));
	mBody.push_back(Image("ui/skin/window_bottom_right.png"));


	mTitle.push_back(Image("ui/skin/window_title_left.png"));
	mTitle.push_back(Image("ui/skin/window_title_middle.png"));
	mTitle.push_back(Image("ui/skin/window_title_right.png"));
}


void Window::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!visible() || !hasFocus()) { return; }

	UIContainer::onMouseDown(button, x, y);

	mMouseDrag = (button == EventHandler::MouseButton::BUTTON_LEFT && pointInRect_f(x, y, rect().x(), rect().y(), rect().width(), WINDOW_TITLE_BAR_HEIGHT));
}


void Window::onMouseUp(EventHandler::MouseButton /*button*/, int /*x*/, int /*y*/)
{
	mMouseDrag = false;
}


void Window::onMouseMotion(int /*x*/, int /*y*/, int dX, int dY)
{
	if (!visible() || !hasFocus()) { return; }

	if (mMouseDrag && !mAnchored)
	{
		position(positionX() + dX, positionY() + dY);
	}
}


void Window::anchored(bool _a)
{
	mAnchored = _a;
}


void Window::show()
{
	Control::show();
	hasFocus(true);
}


void Window::update()
{
	if (!visible()) { return; }

	Renderer& r = Utility<Renderer>::get();

	r.drawImage(mTitle[0], rect().x(), rect().y());
	r.drawImageRepeated(mTitle[1], rect().x() + 4, rect().y(), rect().width() - 8, WINDOW_TITLE_BAR_HEIGHT);
	r.drawImage(mTitle[2], rect().x() + rect().width() - 4, rect().y());

	r.drawImageRect(rect().x(), rect().y() + 20, rect().width(), rect().height() - 20, mBody);

	r.drawText(*WINDOW_TITLE_FONT, text(), rect().x() + 5, rect().y() + 2, 255, 255, 255);

	UIContainer::update();
}
