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
	add(button, {10, 30});
	add(label, {10, 56});
	add(radioButtonGroup, {10, 80});
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
