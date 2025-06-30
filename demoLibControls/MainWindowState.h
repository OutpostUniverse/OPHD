#pragma once

#include "MainWindow.h"

#include <NAS2D/State.h>


class MainWindowState : public NAS2D::State
{
public:
	MainWindowState();

	State* update() override;

private:
	MainWindow mainWindow;
	State* nextState;
};
