#include "NAS2D/NAS2D.h"

#include "utility.h"

#include "../UI/UI.h"


using namespace NAS2D;
using namespace std;


Point_2df MOUSE_POSITION;

bool running = true;


Font*		TINY_FONT = nullptr;

Image*		MOUSE_POINTER = nullptr;

Window*		WINDOW = nullptr;


Rectangle_2d	WINDOW_HANDLE_TOP_L(0, 0, 10, 10);
Rectangle_2d	WINDOW_HANDLE_TOP_R(0, 0, 10, 10);
Rectangle_2d	WINDOW_HANDLE_BOT_L(0, 0, 10, 10);
Rectangle_2d	WINDOW_HANDLE_BOT_R(0, 0, 10, 10);


enum ResizeHandle
{
	RESIZE_TOP_LEFT,
	RESIZE_TOP_RIGHT,
	RESIZE_BOTTOM_LEFT,
	RESIZE_BOTTOM_RIGHT,
	RESIZE_NONE
};


bool		MOUSE_LEFT_DOWN = false;

ResizeHandle HANDLE = RESIZE_NONE;


ResizeHandle getHandle()
{
	// fixme: Kind of ugly.
	if (isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_TOP_L))
		return RESIZE_TOP_LEFT;
	else if (isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_TOP_R))
		return RESIZE_TOP_RIGHT;
	else if (isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_BOT_L))
		return RESIZE_BOTTOM_LEFT;
	else if (isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_BOT_R))
		return RESIZE_BOTTOM_RIGHT;

	return RESIZE_NONE;
}



bool mouseInHandles()
{
	return (WINDOW->hasFocus() &&
			(isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_TOP_L) ||
			isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_TOP_R) ||
			isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_BOT_L) ||
			isPointInRect(MOUSE_POSITION, WINDOW_HANDLE_BOT_R)));
}


void onKeyDown(KeyCode key, KeyModifier mod, bool repeat)
{
	switch (key)
	{
	case KEY_ESCAPE:
		postQuitEvent();
		break;
	default:
		break;
	}
}


void resizeWindow(int dX, int dY)
{
	int x = WINDOW->positionX();
	int y = WINDOW->positionY();
	int width = WINDOW->width();
	int height = WINDOW->height();

	if (HANDLE == RESIZE_TOP_LEFT)
	{
		WINDOW->position(x + dX, y + dY);
		WINDOW->size(clamp(width - dX, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE), clamp(height - dY, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE));
	}

	if (HANDLE == RESIZE_TOP_RIGHT)
	{
		WINDOW->position(x, y + dY);
		WINDOW->size(clamp(width + dX, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE), clamp(height - dY, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE));
	}

	if (HANDLE == RESIZE_BOTTOM_LEFT)
	{
		WINDOW->position(x + dX, y);
		WINDOW->size(clamp(width - dX, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE), clamp(height + dY, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE));

	}

	if (HANDLE == RESIZE_BOTTOM_RIGHT)
	{
		WINDOW->size(clamp(width + dX, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE), clamp(height + dY, MINIMUM_WINDOW_SIZE, MAXIMUM_WINDOW_SIZE));
	}
}

void onMouseMotion(int x, int y, int relX, int relY)
{
	MOUSE_POSITION(static_cast<float>(x), static_cast<float>(y));

	if (MOUSE_LEFT_DOWN && HANDLE != RESIZE_NONE)
	{
		resizeWindow(relX, relY);
	}
}


void onMouseDown(MouseButton button, int x, int y)
{
	if (button == BUTTON_LEFT)
	{
		WINDOW->hasFocus(isPointInRect(MOUSE_POSITION, WINDOW->rect()) || mouseInHandles());
		MOUSE_LEFT_DOWN = true;

		if (mouseInHandles())
		{
			HANDLE = getHandle();
		}
	}
}


void onMouseUp(MouseButton button, int x, int y)
{
	if (button == BUTTON_LEFT)
	{
		MOUSE_LEFT_DOWN = false;
		HANDLE = RESIZE_NONE;
	}
}


void onQuit()
{
	running = false;
}


void initEventHandlers()
{
	cout << "Initializing event handlers... ";
	
	Utility<EventHandler>::get().keyDown().Connect(&onKeyDown);
	Utility<EventHandler>::get().mouseButtonDown().Connect(&onMouseDown);
	Utility<EventHandler>::get().mouseButtonUp().Connect(&onMouseUp);
	Utility<EventHandler>::get().mouseMotion().Connect(&onMouseMotion);
	Utility<EventHandler>::get().quit().Connect(&onQuit);

	cout << "done." << endl;
}


void drawWindowHandles(Window* w)
{
	Renderer& r = Utility<Renderer>::get();

	r.drawBoxFilled(WINDOW_HANDLE_TOP_L, 255, 255, 255);
	r.drawBox(WINDOW_HANDLE_TOP_L, 0, 0, 0);

	r.drawBoxFilled(WINDOW_HANDLE_TOP_R, 255, 255, 255);
	r.drawBox(WINDOW_HANDLE_TOP_R, 0, 0, 0);

	r.drawBoxFilled(WINDOW_HANDLE_BOT_L, 255, 255, 255);
	r.drawBox(WINDOW_HANDLE_BOT_L, 0, 0, 0);

	r.drawBoxFilled(WINDOW_HANDLE_BOT_R, 255, 255, 255);
	r.drawBox(WINDOW_HANDLE_BOT_R, 0, 0, 0);
}


void updateWindowHandles(float dX, float dY)
{
	updateRectPosition(WINDOW_HANDLE_TOP_L, static_cast<int>(dX), static_cast<int>(dY));
	updateRectPosition(WINDOW_HANDLE_TOP_R, static_cast<int>(dX), static_cast<int>(dY));
	updateRectPosition(WINDOW_HANDLE_BOT_L, static_cast<int>(dX), static_cast<int>(dY));
	updateRectPosition(WINDOW_HANDLE_BOT_R, static_cast<int>(dX), static_cast<int>(dY));
}


void windowResized()
{
	updateWindowHandlePositions(WINDOW, WINDOW_HANDLE_TOP_L, WINDOW_HANDLE_TOP_R, WINDOW_HANDLE_BOT_L, WINDOW_HANDLE_BOT_R);
}


int main(int argc, char *argv[])
{
	try
	{
		initNas2d(argv[0], "data", "builder.xml");

		initEventHandlers();

		Renderer& r = Utility<Renderer>::get();

		MOUSE_POINTER = new Image("ui/pointers/normal.png");
		TINY_FONT = new Font("fonts/ui-normal.png", 7, 9, -1);

		WINDOW = new Window();
		WINDOW->size(250, 150);
		WINDOW->position(r.screenCenterX() - 125, r.screenCenterY() - 75);
		WINDOW->font(*TINY_FONT);
		WINDOW->text("Window Title");
		WINDOW->show();
		WINDOW->moved().Connect(&updateWindowHandles);
		WINDOW->resized().Connect(&windowResized);

		updateWindowHandlePositions(WINDOW, WINDOW_HANDLE_TOP_L, WINDOW_HANDLE_TOP_R, WINDOW_HANDLE_BOT_L, WINDOW_HANDLE_BOT_R);

		cout << "Entering main loop..." << endl << endl;

		// Main Loop

		while (running)
		{
			Utility<EventHandler>::get().pump();

			r.drawBoxFilled(0, 0, r.width(), r.height(), 50, 50, 50);

			WINDOW->update();

			if (WINDOW->hasFocus())
				drawWindowHandles(WINDOW);

			r.drawImage(*MOUSE_POINTER, MOUSE_POSITION.x(), MOUSE_POSITION.y());
			r.update();
		}

	}
	catch (Exception& e)
	{
		cout << "EXCEPTION (" << e.getBriefDescription() << "): " << e.getDescription() << endl;
	}

	delete MOUSE_POINTER;
	delete WINDOW;
	delete TINY_FONT;

	return 0;
}
