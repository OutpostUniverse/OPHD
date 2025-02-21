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


void MapControl::onKeyDownEvent(NAS2D::KeyCode keyCode, NAS2D::KeyModifier keyMod, bool repeat)
{
	Event event;
	event.type = Event::Type::KeyDown;
	event.keyCode = keyCode;
	event.keyMod = keyMod;
	event.repeat = repeat;
}
