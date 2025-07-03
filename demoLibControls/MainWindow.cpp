#include "MainWindow.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


MainWindow::MainWindow() :
	Window{"Main Window"},
	button{"Button", {this, &MainWindow::onButtonClick}},
	label{"Label"},
	radioButtonGroup{
		{
			"Option 1",
			"Option 2",
			"Option 3",
		},
		{this, &MainWindow::onRadioButtonSelect}
	},
	image{getImage("ui/interface/product_robodozer.png")},
	progressBar{100, 50},
	rectangle1{NAS2D::Color::Green, {108, 1}},
	rectangle2{NAS2D::Color::Red, {108, 5}}
{
	const auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	size(renderer.size());

	button.size(button.size() + NAS2D::Vector{8, 2});

	listBox1.size({80, 100});
	listBox1.add("Item 1");
	listBox1.add("Item 2");
	listBox1.add("Item 3");
	listBox1.add("Item 4");
	listBox1.add("Item 5");
	listBox1.add("Item 6");

	listBox2.size({60, 100});
	listBox2.add("Item 1");
	listBox2.add("Item 2");
	listBox2.add("Item 3");

	comboBox1.size({80, 20});
	comboBox1.addItem("Item1");
	comboBox1.addItem("Item2");
	comboBox1.addItem("Item3");
	comboBox1.addItem("Item4");
	comboBox1.addItem("Item5");
	comboBox1.addItem("Item6");

	comboBox2.size({60, 20});
	comboBox2.addItem("Item1");
	comboBox2.addItem("Item2");
	comboBox2.addItem("Item3");

	add(button, {10, 30});
	add(label, {10, 56});
	add(radioButtonGroup, {10, 80});
	add(listBox1, {130, 30});
	add(listBox2, {220, 30});
	add(comboBox1, {130, 150});
	add(comboBox2, {220, 150});
	add(image, {10, 140});
	add(progressBar, {10, 270});
	add(rectangle1, {10, 300});
	add(rectangle2, {10, 310});
}


void MainWindow::onButtonClick()
{
	label.text("Button clicked");
}


void MainWindow::onRadioButtonSelect(std::size_t index)
{
	label.text("RadioButton : " + std::to_string(index));
}
