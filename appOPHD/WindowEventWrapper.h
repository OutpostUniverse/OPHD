#pragma once

#include "Common.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Configuration.h>
#include <NAS2D/Renderer/Renderer.h>


/**
 * Function class used to get around the limitation in NAS2D's EventHandler
 *			Delegate system that prevents connecting stand alone functions.
 *
 * This class handles updating the configuration settings as various window
 * events occur.
 */
class WindowEventWrapper
{
public:
	WindowEventWrapper()
	{
		NAS2D::Utility<NAS2D::EventHandler>::get().windowMaximized().connect({this, &WindowEventWrapper::onWindowMaximized});
		NAS2D::Utility<NAS2D::EventHandler>::get().windowRestored().connect({this, &WindowEventWrapper::onWindowRestored});
		NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect({this, &WindowEventWrapper::onWindowResized});
	}

	~WindowEventWrapper()
	{
		NAS2D::Utility<NAS2D::EventHandler>::get().windowMaximized().disconnect({this, &WindowEventWrapper::onWindowMaximized});
		NAS2D::Utility<NAS2D::EventHandler>::get().windowRestored().disconnect({this, &WindowEventWrapper::onWindowRestored});
		NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().disconnect({this, &WindowEventWrapper::onWindowResized});
	}

private:
	void onWindowMaximized()
	{
		NAS2D::Utility<NAS2D::Configuration>::get()["options"].set("maximized", true);
	}

	void onWindowRestored()
	{
		NAS2D::Utility<NAS2D::Configuration>::get()["options"].set("maximized", false);
	}

	void onWindowResized(NAS2D::Vector<int> newSize)
	{
		if (NAS2D::Utility<NAS2D::Renderer>::get().isMaximized()) { return; }

		auto& configuration = NAS2D::Utility<NAS2D::Configuration>::get();
		auto& graphics = configuration["graphics"];
		graphics.set("screenwidth", newSize.x);
		graphics.set("screenheight", newSize.y);
	}
};
