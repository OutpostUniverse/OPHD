#include "WindowEventWrapper.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Configuration.h>
#include <NAS2D/Renderer/Renderer.h>


WindowEventWrapper::WindowEventWrapper()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().windowMaximized().connect({this, &WindowEventWrapper::onWindowMaximized});
	NAS2D::Utility<NAS2D::EventHandler>::get().windowRestored().connect({this, &WindowEventWrapper::onWindowRestored});
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect({this, &WindowEventWrapper::onWindowResized});
}


WindowEventWrapper::~WindowEventWrapper()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().windowMaximized().disconnect({this, &WindowEventWrapper::onWindowMaximized});
	NAS2D::Utility<NAS2D::EventHandler>::get().windowRestored().disconnect({this, &WindowEventWrapper::onWindowRestored});
	NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().disconnect({this, &WindowEventWrapper::onWindowResized});
}


void WindowEventWrapper::onWindowMaximized()
{
	NAS2D::Utility<NAS2D::Configuration>::get()["options"].set("maximized", true);
}


void WindowEventWrapper::onWindowRestored()
{
	NAS2D::Utility<NAS2D::Configuration>::get()["options"].set("maximized", false);
}


void WindowEventWrapper::onWindowResized(NAS2D::Vector<int> newSize)
{
	if (NAS2D::Utility<NAS2D::Renderer>::get().isMaximized()) { return; }

	auto& configuration = NAS2D::Utility<NAS2D::Configuration>::get();
	auto& graphics = configuration["graphics"];
	graphics.set("screenwidth", newSize.x);
	graphics.set("screenheight", newSize.y);
}
