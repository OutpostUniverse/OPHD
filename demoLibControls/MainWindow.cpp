#include "MainWindow.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


MainWindow::MainWindow() :
	Window{"Main Window"},
	button{"Button"},
	label{"Label"}
{
	const auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	size(renderer.size());

	add(button, {10, 30});
	add(label, {70, 30});
}
