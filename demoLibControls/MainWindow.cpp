#include "MainWindow.h"

#include <libControls/LoadRectangleSkin.h>

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


MainWindow::MainWindow() :
	Window{"Main Window"},
	label{"Label"},
	labelFontBold{"Big Font"},
	labelFontBoldRed{"Big Red Font"},
	labelInfo{"Info:"},
	textFieldNumbersMax{4},
	textFieldMax{4},
	imageButton{"ui/icons/mine.png"},
	buttonSkin{
		loadRectangleSkin("ui/skin/button_normal"),
		loadRectangleSkin("ui/skin/button_hover"),
		loadRectangleSkin("ui/skin/button_pressed"),
	},
	button{"Button", {this, &MainWindow::onButtonClick}},
	buttonWithSkin{buttonSkin, "Skin", {this, &MainWindow::onButtonClick}},
	buttonEmpty{"", {this, &MainWindow::onButtonClick}},
	buttonImage{imageButton, {this, &MainWindow::onButtonClick}},
	buttonAll{buttonSkin, &imageButton, getDefaultFont(), "ABC          DEF", {this, &MainWindow::onButtonClick}},
	radioButtonGroup{
		{
			"Option 1",
			"Option 2",
			"Option 3",
		},
		{this, &MainWindow::onRadioButtonSelect}
	},
	checkBox1{"CheckBox 1", {this, &MainWindow::onCheckBoxClick}},
	checkBox2{"CheckBox 2", {this, &MainWindow::onCheckBoxClick}},
	checkBox3{"CheckBox 3", {this, &MainWindow::onCheckBoxClick}},
	listBox1{{this, &MainWindow::onListBoxSelect}},
	listBox2{{this, &MainWindow::onListBoxSelect}},
	comboBox1{{this, &MainWindow::onComboBoxSelect}},
	comboBox2{{this, &MainWindow::onComboBoxSelect}},
	scrollBarHorizontalMax0{ScrollBar::ScrollBarType::Horizontal, 1, {this, &MainWindow::onScrollBarChange}},
	scrollBarHorizontalMax1{ScrollBar::ScrollBarType::Horizontal, 1, {this, &MainWindow::onScrollBarChange}},
	scrollBarHorizontal{ScrollBar::ScrollBarType::Horizontal, 1, {this, &MainWindow::onScrollBarChange}},
	scrollBarVerticalMax0{ScrollBar::ScrollBarType::Vertical, 1, {this, &MainWindow::onScrollBarChange}},
	scrollBarVerticalMax1{ScrollBar::ScrollBarType::Vertical, 1, {this, &MainWindow::onScrollBarChange}},
	scrollBarVertical{ScrollBar::ScrollBarType::Vertical, 1, {this, &MainWindow::onScrollBarChange}},
	rectangle1{NAS2D::Color::Green, {80, 1}},
	rectangle2{NAS2D::Color::Red, {80, 5}},
	progressBarUndefined{0, 0},
	progressBar0{100, 0},
	progressBar10{100, 10},
	progressBar50{100, 50},
	progressBar90{100, 90},
	progressBar100{100, 100},
	image{getImage("ui/interface/product_robodozer.png")}
{
	const auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	size(renderer.size());

	labelFontBold.font(&getDefaultFontBold());
	labelFontBoldRed.font(&getDefaultFontBold());
	labelFontBoldRed.color(NAS2D::Color::Red);

	textField.width(130);
	textField.text("This is a text field");
	textFieldNumbers.width(130);
	textFieldNumbers.numbersOnly(true);
	textFieldNumbers.text("1234567890");
	textFieldNumbersMax.numbersOnly(true);
	textFieldNumbersMax.text("1234");
	textFieldMax.text("Max4");
	textFieldOverflow.size(textFieldMax.size());
	textFieldOverflow.text("Overflowing text");

	textArea.size({100, 100});
	textArea.text("This is some\nTextArea\ntext.");

	listBox1.size({80, 100});
	listBox1.add("Item 1");
	listBox1.add("Item 2");
	listBox1.add("Item 3");
	listBox1.add("Item 4");
	listBox1.add("Item 5");
	listBox1.add("Item 6");

	comboBox1.width(80);
	comboBox1.addItem("Item1");
	comboBox1.addItem("Item2");
	comboBox1.addItem("Item3");
	comboBox1.addItem("Item4");
	comboBox1.addItem("Item5");
	comboBox1.addItem("Item6");

	listBox2.size({60, 100});
	listBox2.add("Item 1");
	listBox2.add("Item 2");
	listBox2.add("Item 3");

	comboBox2.width(60);
	comboBox2.addItem("Item1");
	comboBox2.addItem("Item2");
	comboBox2.addItem("Item3");

	scrollBarHorizontalMax1.max(1);
	scrollBarHorizontal.max(200);
	scrollBarVerticalMax1.max(1);
	scrollBarVertical.max(200);

	scrollBarHorizontalMax0.size({70, 20});
	scrollBarHorizontalMax1.size({70, 20});
	scrollBarHorizontal.size({70, 20});
	scrollBarVerticalMax0.size({20, 200});
	scrollBarVerticalMax1.size({20, 200});
	scrollBarVertical.size({20, 200});

	progressBarUndefined.width(80);
	progressBar0.width(80);
	progressBar10.width(80);
	progressBar50.width(80);
	progressBar90.width(80);
	progressBar100.width(80);

	add(label, {10, 30});
	add(labelFontBold, {10, 60});
	add(labelFontBoldRed, {10, 90});
	add(labelInfo, {10, 120});

	add(textField, {150, 30});
	add(textFieldNumbers, {150, 60});
	add(textFieldNumbersMax, {150, 90});
	add(textFieldMax, {150, 120});
	add(textFieldOverflow, {150, 150});
	add(textArea, {150, 180});

	add(button, {300, 30});
	add(buttonWithSkin, {300, 60});
	add(buttonEmpty, {300, 90});
	add(buttonImage, {300, 120});
	add(buttonAll, {300, 165});

	add(radioButtonGroup, {400, 30});
	add(checkBox1, {400, 100});
	add(checkBox2, {400, 120});
	add(checkBox3, {400, 140});

	add(listBox1, {500, 30});
	add(comboBox1, {500, 150});

	add(listBox2, {600, 30});
	add(comboBox2, {600, 150});

	add(scrollBarHorizontalMax0, {670, 30});
	add(scrollBarHorizontalMax1, {670, 60});
	add(scrollBarHorizontal, {670, 90});
	add(scrollBarVerticalMax0, {720, 120});
	add(scrollBarVerticalMax1, {695, 120});
	add(scrollBarVertical, {670, 120});

	add(rectangle1, {750, 30});
	add(rectangle2, {750, 40});
	add(progressBarUndefined, {750, 50});
	add(progressBar0, {750, 80});
	add(progressBar10, {750, 110});
	add(progressBar50, {750, 140});
	add(progressBar90, {750, 170});
	add(progressBar100, {750, 200});

	add(image, {850, 30});
}


void MainWindow::onButtonClick()
{
	labelInfo.text("Button clicked");
}


void MainWindow::onRadioButtonSelect(std::size_t index)
{
	labelInfo.text("RadioButton : " + std::to_string(index));
}


void MainWindow::onCheckBoxClick()
{
	labelInfo.text("CheckBox clicked");
}


void MainWindow::onListBoxSelect()
{
	labelInfo.text("ListBox select");
}


void MainWindow::onComboBoxSelect()
{
	labelInfo.text("ComboBox select");
}


void MainWindow::onScrollBarChange(int newValue)
{
	labelInfo.text("ScrollBar: " + std::to_string(newValue));
}
