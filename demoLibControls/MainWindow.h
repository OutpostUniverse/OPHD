#pragma once

#include <libControls/Window.h>

#include <libControls/Button.h>
#include <libControls/Label.h>
#include <libControls/RadioButtonGroup.h>


class MainWindow : public Window
{
public:
	MainWindow();

protected:
	void onButtonClick();
	void onRadioButtonClick();

private:
	Button button;
	Label label;
	RadioButtonGroup radioButtonGroup;
};
