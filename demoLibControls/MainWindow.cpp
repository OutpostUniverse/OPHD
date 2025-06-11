#include "MainWindow.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


MainWindow::MainWindow() :
	Window{"Main Window"},
	button{"Button", {this, &MainWindow::onButtonClick}},
	label{"Label: Waiting for button"}
{
	const auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	size(renderer.size());

	button.size(button.size() + NAS2D::Vector{8, 2});
	add(button, {10, 30});
	add(label, {70, 30});
}


void MainWindow::onButtonClick()
{
	label.text("Label: Button was clicked");
}
