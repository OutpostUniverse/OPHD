#include "KeyboardInputHandler.h"

#include <NAS2D/Math/Vector.h>
#include <OPHD/Map/MapOffset.h>
#include <OPHD/DirectionOffset.h>
#include <OPHD/States/MapViewState.h>


KeyboardInputHandler::KeyboardInputHandler(MapView& mapView, NAS2D::Signal<>* reportsUiSignal) :
	mMapView(&mapView)
{
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_w] = new MoveCommand(*mMapView, DirectionNorthWest * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_UP] = new MoveCommand(*mMapView, DirectionNorthWest * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_s] = new MoveCommand(*mMapView, DirectionSouthEast * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_DOWN] = new MoveCommand(*mMapView, DirectionSouthEast * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_a] = new MoveCommand(*mMapView, DirectionSouthWest * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_LEFT] = new MoveCommand(*mMapView, DirectionSouthWest * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_d] = new MoveCommand(*mMapView, DirectionNorthEast * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift][NAS2D::EventHandler::KeyCode::KEY_RIGHT] = new MoveCommand(*mMapView, DirectionNorthEast * 5);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_w] = new MoveCommand(*mMapView, DirectionNorthWest);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_UP] = new MoveCommand(*mMapView, DirectionNorthWest);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_s] = new MoveCommand(*mMapView, DirectionSouthEast);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_DOWN] = new MoveCommand(*mMapView, DirectionSouthEast);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_a] = new MoveCommand(*mMapView, DirectionSouthWest);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_LEFT] = new MoveCommand(*mMapView, DirectionSouthWest);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_d] = new MoveCommand(*mMapView, DirectionNorthEast);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_RIGHT] = new MoveCommand(*mMapView, DirectionNorthEast);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_F1] = new SignalCommand(reportsUiSignal);
}


void KeyboardInputHandler::handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier keyModifiers)
{
	auto keyModifiersFitered = NAS2D::EventHandler::KeyModifier::None;
	if (NAS2D::Utility<NAS2D::EventHandler>::get().shift(keyModifiers))
	{
		keyModifiersFitered |= NAS2D::EventHandler::KeyModifier::Shift;
	}

	if (NAS2D::Utility<NAS2D::EventHandler>::get().control(keyModifiers))
	{
		keyModifiersFitered |= NAS2D::EventHandler::KeyModifier::Ctrl;
	}

	if (mKeyModifierMap.find(keyModifiersFitered) != mKeyModifierMap.end() && mKeyModifierMap[keyModifiersFitered].find(key) != mKeyModifierMap[keyModifiersFitered].end())
	{
		mKeyModifierMap[keyModifiersFitered][key]->execute();
	}
}
