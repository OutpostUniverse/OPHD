#pragma once
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>

#include "../Constants/Strings.h"
#include "../Map/MapView.h"

class KeyboardInputHandler
{
public:
	KeyboardInputHandler(MapView& mapView);
	void handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod);

private:
	MapView* mMapView;
};
