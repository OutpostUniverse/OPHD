#include "MainWindow.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


MainWindow::MainWindow() :
	Window{"Main Window"},
	button{"Button", {this, &MainWindow::onButtonClick}},
	label{"Label: Waiting for button"},
	radioButtonGroup{
		{
			"Option 1",
			"Option 2",
			"Option 3",
		},
		{this, &MainWindow::onRadioButtonSelect}
	},
	image{getImage("ui/interface/product_robodozer.png")}
{
	const auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	size(renderer.size());

	button.size(button.size() + NAS2D::Vector{8, 2});
	add(button, {10, 30});
	add(label, {70, 30});
	add(radioButtonGroup, {10, 60});
	add(image, {10, 120});
}


void MainWindow::onButtonClick()
{
	label.text("Label: Button was clicked");
}


void MainWindow::onRadioButtonSelect(std::size_t index)
{
	label.text("Label: RadioButton was selected : " + std::to_string(index));
}
