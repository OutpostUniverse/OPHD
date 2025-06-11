#pragma once

#include <libControls/Window.h>

#include <libControls/Button.h>
#include <libControls/Label.h>


class MainWindow : public Window
{
public:
	MainWindow();

private:
	Button button;
	Label label;
};
