#include "MapControl.h"

#include "../UI/CheatMenu.h"
#include <libControls/WindowStack.h>

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>

MapControl::MapControl(CheatMenu& cheatMenu, WindowStack& windowStack) : 
mCheatMenu{cheatMenu}, 
mWindowStack{windowStack}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.keyDown().connect({this, &MapControl::onKeyDownEvent});
	addChild(&mCheatMenu);
}


MapControl::~MapControl()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.keyDown().disconnect({this, &MapControl::onKeyDownEvent});
}


void MapControl::handleEvent(Event& event)
{
	dispatchEvent(event);
	if(event.eventHandled) { return; }

	if(event.type == Event::Type::KeyDown)
	{
		onKeyDown(event);
	}
}


void MapControl::onKeyDownEvent(NAS2D::KeyCode keyCode, NAS2D::KeyModifier keyMod, bool repeat)
{
	Event event;
	event.type = Event::Type::KeyDown;
	event.keyCode = keyCode;
	event.keyMod = keyMod;
	event.repeat = repeat;

	handleEvent(event);
}

void MapControl::onKeyDown(Event& event)
{
	switch (event.keyCode)
	{
		case NAS2D::KeyCode::F10:
			if (NAS2D::Utility<NAS2D::EventHandler>::get().control(event.keyMod) && NAS2D::Utility<NAS2D::EventHandler>::get().shift(event.keyMod))
			{
				mCheatMenu.show();
				mWindowStack.bringToFront(&mCheatMenu);
			}
			break;

		default:
			break;
	}
}
