#include "MainWindow.h"

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
	button{"Button", {this, &MainWindow::onButtonClick}},
	radioButtonGroup{
		{
			"Option 1",
			"Option 2",
			"Option 3",
		},
		{this, &MainWindow::onRadioButtonSelect}
	},
	checkBox1{"CheckBox 1"},
	checkBox2{"CheckBox 2"},
	checkBox3{"CheckBox 3"},
	listBox1{{this, &MainWindow::onListBoxSelect}},
	listBox2{{this, &MainWindow::onListBoxSelect}},
	comboBox1{{this, &MainWindow::onComboBoxSelect}},
	comboBox2{{this, &MainWindow::onComboBoxSelect}},
	rectangle1{NAS2D::Color::Green, {80, 1}},
	rectangle2{NAS2D::Color::Red, {80, 5}},
	progressBar{100, 50},
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
	textFieldOverflow.size(button.size());
	textFieldOverflow.text("Overflowing text");

	button.size(button.size() + NAS2D::Vector{8, 2});

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

	progressBar.width(80);

	add(label, {10, 30});
	add(labelFontBold, {10, 60});
	add(labelFontBoldRed, {10, 90});
	add(labelInfo, {10, 120});

	add(textField, {150, 30});
	add(textFieldNumbers, {150, 60});
	add(textFieldNumbersMax, {150, 90});
	add(textFieldMax, {150, 120});
	add(textFieldOverflow, {150, 150});

	add(button, {300, 30});

	add(radioButtonGroup, {400, 30});
	add(checkBox1, {400, 100});
	add(checkBox2, {400, 120});
	add(checkBox3, {400, 140});

	add(listBox1, {500, 30});
	add(comboBox1, {500, 150});

	add(listBox2, {600, 30});
	add(comboBox2, {600, 150});

	add(rectangle1, {700, 30});
	add(rectangle2, {700, 40});
	add(progressBar, {700, 50});

	add(image, {800, 30});
}


void MainWindow::onButtonClick()
{
	labelInfo.text("Button clicked");
}


void MainWindow::onRadioButtonSelect(std::size_t index)
{
	labelInfo.text("RadioButton : " + std::to_string(index));
}


void MainWindow::onListBoxSelect()
{
	labelInfo.text("ListBox select");
}


void MainWindow::onComboBoxSelect()
{
	labelInfo.text("ComboBox select");
}
