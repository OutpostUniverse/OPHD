#include "MainWindow.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


MainWindow::MainWindow() :
	Window{"Main Window"},
	button{"Button", {this, &MainWindow::onButtonClick}},
	label{"Label: Waiting for button"},
	radioButtonGroup{
		{
			{"Option 1", {this, &MainWindow::onRadioButtonClick}},
			{"Option 2", {this, &MainWindow::onRadioButtonClick}},
			{"Option 3", {this, &MainWindow::onRadioButtonClick}},
		}
	}
{
	const auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	size(renderer.size());

	button.size(button.size() + NAS2D::Vector{8, 2});
	add(button, {10, 30});
	add(label, {70, 30});
	add(radioButtonGroup, {10, 60});
}


void MainWindow::onButtonClick()
{
	label.text("Label: Button was clicked");
}


void MainWindow::onRadioButtonClick()
{
	label.text("Label: RadioButton was clicked");
}
