#include "Window.h"

const int WINDOW_TITLE_BAR_HEIGHT = 19;

Window::Window() : mBold("fonts/ui-bold.png", 7, 9, 0), mMouseDrag(false)
{
	EventHandler& e = Utility<EventHandler>::get();
	Utility<EventHandler>::get().mouseButtonDown().Connect(this, &Window::onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Connect(this, &Window::onMouseUp);
	Utility<EventHandler>::get().mouseMotion().Connect(this, &Window::onMouseMotion);
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

	if (mMouseDrag)
		position(positionX() + dX, positionY() + dY);
}



void Window::update()
{
	if (!visible())
		return;

	Renderer& r = Utility<Renderer>::get();

	// Body
	r.drawBoxFilled(rect(), COLOR_SILVER.red(), COLOR_SILVER.green(), COLOR_SILVER.blue());
	r.drawBox(rect(), 0, 0, 0);

	// Title Bar
	r.drawBoxFilled(rect().x(), rect().y(), rect().w(), WINDOW_TITLE_BAR_HEIGHT, COLOR_SILVER.red() + 25, COLOR_SILVER.green() + 25, COLOR_SILVER.blue() + 25);
	r.drawBox(rect().x(), rect().y(), rect().w(), WINDOW_TITLE_BAR_HEIGHT, 0, 0, 0);
	r.drawText(mBold, text(), rect().x() + 5, rect().y() + 5, 0, 0, 0);

	UIContainer::update();
}
