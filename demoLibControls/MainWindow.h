#pragma once

#include <libControls/Window.h>

#include <libControls/Button.h>
#include <libControls/Label.h>


class MainWindow : public Window
{
public:
	MainWindow();

protected:
	void onButtonClick();

private:
	Button button;
	Label label;
};
