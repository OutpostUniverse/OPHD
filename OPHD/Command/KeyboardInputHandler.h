#pragma once
#include <unordered_map>

#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Signal/Signal.h>

#include "../Constants/Strings.h"
#include "../Map/MapView.h"
#include "../DirectionOffset.h"

#include "Command.h"
#include "Commands.cpp"

class KeyboardInputHandler
{
public:
	KeyboardInputHandler(MapView& mapView, NAS2D::Signal<>* reportsUiSignal);
	void handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier keyModifiers);

private:
	MapView* mMapView;
	NAS2D::Signal<>* mReportsUiSignal;
	NAS2D::EventHandler::KeyModifier mKeyModifiers;

	std::unordered_map<NAS2D::EventHandler::KeyCode, Command*> mKeyCommandMap;

	Command* mNullCommand = new NullCommand();
};
