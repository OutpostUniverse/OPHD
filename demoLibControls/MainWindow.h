#pragma once

#include <libControls/Window.h>

#include <libControls/Button.h>
#include <libControls/Image.h>
#include <libControls/Label.h>
#include <libControls/ProgressBar.h>
#include <libControls/RadioButtonGroup.h>
#include <libControls/Rectangle.h>
#include <libControls/ListBox.h>
#include <libControls/ComboBox.h>
#include <libControls/TextField.h>


class MainWindow : public Window
{
public:
	MainWindow();

protected:
	void onButtonClick();
	void onRadioButtonSelect(std::size_t index);

private:
	Button button;
	Label label;
	RadioButtonGroup radioButtonGroup;
	Image image;
	ProgressBar progressBar;
	Rectangle rectangle1;
	Rectangle rectangle2;
	ListBox<> listBox1;
	ListBox<> listBox2;
	ComboBox comboBox1;
	ComboBox comboBox2;
	TextField textField;
};
