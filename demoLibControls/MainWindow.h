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
	void onListBoxSelect();
	void onComboBoxSelect();

private:
	Label label;
	Label labelFontBold;
	Label labelFontBoldRed;
	Label labelInfo;
	TextField textField;
	TextField textFieldNumbers;
	TextField textFieldNumbersMax;
	TextField textFieldMax;
	TextField textFieldOverflow;
	Button button;
	RadioButtonGroup radioButtonGroup;
	ListBox<> listBox1;
	ListBox<> listBox2;
	ComboBox comboBox1;
	ComboBox comboBox2;
	Rectangle rectangle1;
	Rectangle rectangle2;
	ProgressBar progressBar;
	Image image;
};
