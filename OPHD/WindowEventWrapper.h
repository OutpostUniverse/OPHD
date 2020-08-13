#pragma once

#include "Common.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Configuration.h>


/**
 * \brief	Function class used to get around the limitation in NAS2D's EventHandler
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
		NAS2D::Utility<NAS2D::EventHandler>::get().windowMaximized().connect(this, &WindowEventWrapper::onWindowMaximized);
		NAS2D::Utility<NAS2D::EventHandler>::get().windowRestored().connect(this, &WindowEventWrapper::onWindowRestored);
		NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().connect(this, &WindowEventWrapper::onWindowResized);
	}

	~WindowEventWrapper()
	{
		NAS2D::Utility<NAS2D::EventHandler>::get().windowMaximized().disconnect(this, &WindowEventWrapper::onWindowMaximized);
		NAS2D::Utility<NAS2D::EventHandler>::get().windowRestored().disconnect(this, &WindowEventWrapper::onWindowRestored);
		NAS2D::Utility<NAS2D::EventHandler>::get().windowResized().disconnect(this, &WindowEventWrapper::onWindowResized);
	}

private:
	void onWindowMaximized()
	{
		NAS2D::Utility<NAS2D::Configuration>::get().option("maximized", "true");
	}

	void onWindowRestored()
	{
		NAS2D::Utility<NAS2D::Configuration>::get().option("maximized", "false");
	}

	void onWindowResized(int w, int h)
	{
		if (windowMaximized()) { return; }

		auto& configuration = NAS2D::Utility<NAS2D::Configuration>::get();
		auto& graphics = configuration["graphics"];
		graphics.set("screenwidth", w);
		graphics.set("screenheight", h);
	}
};
