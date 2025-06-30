#include "MainWindowState.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/EnumKeyCode.h>


MainWindowState::MainWindowState() :
	nextState{this}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.keyDown().connect({this, &MainWindowState::onKeyDown});
}


NAS2D::State* MainWindowState::update()
{
	mainWindow.update();

	return nextState;
}


void MainWindowState::onKeyDown(NAS2D::KeyCode keyCode, NAS2D::KeyModifier /*keyModifier*/, bool /*repeat*/)
{
	if (keyCode == NAS2D::KeyCode::Escape)
	{
		nextState = nullptr;
	}
}
