#pragma once

#include "MainWindow.h"

#include <NAS2D/State.h>


class MainWindowState : public NAS2D::State
{
public:
	MainWindowState();

	State* update() override;

protected:
	void onKeyDown(NAS2D::KeyCode keyCode, NAS2D::KeyModifier keyModifier, bool repeat);

private:
	MainWindow mainWindow;
	State* nextState;
};
