#include "MapControl.h"

#include <NAS2D/Utility.h>

MapControl::MapControl()
{
}


MapControl::~MapControl()
{
}

void MapControl::onKeyDownEvent(NAS2D::KeyCode keyCode, NAS2D::KeyModifier keyMod, bool repeat)
{
	Event event;
	event.type = Event::Type::KeyDown;
	event.keyCode = keyCode;
	event.keyMod = keyMod;
	event.repeat = repeat;
}
