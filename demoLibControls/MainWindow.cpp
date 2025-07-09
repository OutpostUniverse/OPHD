#include "MainWindow.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


MainWindow::MainWindow() :
	Window{"Main Window"},
	label{"Label"},
	button{"Button", {this, &MainWindow::onButtonClick}},
	radioButtonGroup{
		{
			"Option 1",
			"Option 2",
			"Option 3",
		},
		{this, &MainWindow::onRadioButtonSelect}
	},
	rectangle1{NAS2D::Color::Green, {80, 1}},
	rectangle2{NAS2D::Color::Red, {80, 5}},
	progressBar{100, 50},
	image{getImage("ui/interface/product_robodozer.png")}
{
	const auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	size(renderer.size());

	textField.size(button.size());
	textField.text("Overflowing text");

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

	add(textField, {150, 30});

	add(button, {300, 30});

	add(radioButtonGroup, {400, 30});

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
	label.text("Button clicked");
}


void MainWindow::onRadioButtonSelect(std::size_t index)
{
	label.text("RadioButton : " + std::to_string(index));
}
