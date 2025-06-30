#include "MainWindowState.h"


MainWindowState::MainWindowState() :
	nextState{this}
{
}


NAS2D::State* MainWindowState::update()
{
	mainWindow.update();

	return nextState;
}
