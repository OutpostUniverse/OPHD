#include "Window.h"

const int WINDOW_TITLE_BAR_HEIGHT = 20;

Window::Window() : mBold("fonts/ui-bold.png", 7, 9, 0), mMouseDrag(false), mAnchored(false)
{
	EventHandler& e = Utility<EventHandler>::get();
	Utility<EventHandler>::get().mouseButtonDown().Connect(this, &Window::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Connect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().Connect(this, &Window::onMouseMotion);

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
	Utility<EventHandler>::get().mouseButtonDown().Disconnect(this, &Window::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Disconnect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().Disconnect(this, &Window::onMouseMotion);
}


void Window::onMouseDown(MouseButton button, int x, int y)
{
	if (!visible() || !hasFocus())
		return;

	if(button == BUTTON_LEFT && isPointInRect(x, y, rect().x(), rect().y(), rect().w(), WINDOW_TITLE_BAR_HEIGHT))
		mMouseDrag = true;
}


void Window::onMouseUp(MouseButton button, int x, int y)
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


void Window::update()
{
	if (!visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	r.drawImage(mTitle[0], rect().x(), rect().y());
	r.drawImageRepeated(mTitle[1], rect().x() + 4, rect().y(), rect().w() - 8, WINDOW_TITLE_BAR_HEIGHT);
	r.drawImage(mTitle[2], rect().x() + rect().w() - 4, rect().y());

	r.drawImageRect(rect().x(), rect().y() + 20, rect().w(), rect().h() - 20, mBody);
	
	r.drawText(mBold, text(), rect().x() + 5, rect().y() + 5, 255, 255, 255);

	UIContainer::update();
}
