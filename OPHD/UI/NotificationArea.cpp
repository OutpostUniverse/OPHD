#include "NotificationArea.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

using namespace NAS2D;


NotificationArea::NotificationArea()
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().connect(this, &NotificationArea::onMouseDown);
	eventhandler.mouseMotion().connect(this, &NotificationArea::onMouseMove);
}


NotificationArea::~NotificationArea()
{
	auto& eventhandler = Utility<EventHandler>::get();

	eventhandler.mouseButtonDown().disconnect(this, &NotificationArea::onMouseDown);
	eventhandler.mouseMotion().disconnect(this, &NotificationArea::onMouseMove);
}


void NotificationArea::push(Notification notification)
{

}


void NotificationArea::onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y)
{

}


void NotificationArea::onMouseMove(int x, int y, int deltaX, int deltaY)
{

}

void NotificationArea::update()
{
	auto& renderer = Utility<Renderer>::get();

	renderer.drawBox(rect());
}
