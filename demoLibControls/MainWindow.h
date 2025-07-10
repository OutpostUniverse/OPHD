#pragma once

#include <libControls/Window.h>

#include <libControls/Label.h>
#include <libControls/TextField.h>
#include <libControls/Button.h>
#include <libControls/RadioButtonGroup.h>
#include <libControls/CheckBox.h>
#include <libControls/ListBox.h>
#include <libControls/ComboBox.h>
#include <libControls/Rectangle.h>
#include <libControls/ProgressBar.h>
#include <libControls/Image.h>

#include <NAS2D/Resource/Image.h>


class MainWindow : public Window
{
public:
	MainWindow();

protected:
	void onButtonClick();
	void onRadioButtonSelect(std::size_t index);
	void onCheckBoxClick();
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
	NAS2D::Image imageButton;
	Button::ButtonSkin buttonSkin;
	Button button;
	Button buttonWithSkin;
	Button buttonEmpty;
	Button buttonImage;
	Button buttonAll;
	RadioButtonGroup radioButtonGroup;
	CheckBox checkBox1;
	CheckBox checkBox2;
	CheckBox checkBox3;
	ListBox<> listBox1;
	ListBox<> listBox2;
	ComboBox comboBox1;
	ComboBox comboBox2;
	Rectangle rectangle1;
	Rectangle rectangle2;
	ProgressBar progressBarUndefined;
	ProgressBar progressBar0;
	ProgressBar progressBar10;
	ProgressBar progressBar50;
	ProgressBar progressBar90;
	ProgressBar progressBar100;
	Image image;
};
