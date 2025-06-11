#include "MainWindowState.h"


NAS2D::State* MainWindowState::update()
{
	mainWindow.update();

	return this;
}
