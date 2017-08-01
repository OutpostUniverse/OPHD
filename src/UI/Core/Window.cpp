#include "Window.h"

#include "../../Common.h"

const float WINDOW_TITLE_BAR_HEIGHT = 20.0f;


Window::Window() : mBold("fonts/opensans-bold.ttf", 10), mMouseDrag(false), mAnchored(false)
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &Window::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().connect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().connect(this, &Window::onMouseMotion);

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


Window::~Window()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &Window::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().disconnect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &Window::onMouseMotion);
}


void Window::onMouseDown(EventHandler::MouseButton button, int x, int y)
{
	if (!visible() || !hasFocus())
		return;

	if(button == EventHandler::BUTTON_LEFT && pointInRect_f(x, y, rect().x(), rect().y(), rect().width(), WINDOW_TITLE_BAR_HEIGHT))
		mMouseDrag = true;
}


void Window::onMouseUp(EventHandler::MouseButton button, int x, int y)
{
	mMouseDrag = false;
}


void Window::onMouseMotion(int x, int y, int dX, int dY)
{
	if (!visible() || !hasFocus())
		return;

	if (mMouseDrag && !mAnchored)
		position(positionX() + dX, positionY() + dY);
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
	if (!visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	r.drawImage(mTitle[0], rect().x(), rect().y());
	r.drawImageRepeated(mTitle[1], rect().x() + 4, rect().y(), rect().width() - 8, WINDOW_TITLE_BAR_HEIGHT);
	r.drawImage(mTitle[2], rect().x() + rect().width() - 4, rect().y());

	r.drawImageRect(rect().x(), rect().y() + 20, rect().width(), rect().height() - 20, mBody);

	r.drawText(mBold, text(), rect().x() + 5, rect().y() + 2, 255, 255, 255);

	UIContainer::update();
}
