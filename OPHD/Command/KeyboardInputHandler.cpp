#include "KeyboardInputHandler.h"

#include <NAS2D/Math/Vector.h>
#include <OPHD/Map/MapOffset.h>
#include <OPHD/DirectionOffset.h>
#include <OPHD/States/MapViewState.h>


KeyboardInputHandler::KeyboardInputHandler(MapView& mapView, NAS2D::Signal<>* reportsUiSignal, CheatMenu* cheatMenu, WindowStack* windowStack) :
	mMapView(&mapView)
{
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_w] = new MoveCommand(*mMapView, DirectionNorthWest, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_UP] = new MoveCommand(*mMapView, DirectionNorthWest, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_s] = new MoveCommand(*mMapView, DirectionSouthEast, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_DOWN] = new MoveCommand(*mMapView, DirectionSouthEast, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_a] = new MoveCommand(*mMapView, DirectionSouthWest, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_LEFT] = new MoveCommand(*mMapView, DirectionSouthWest, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_d] = new MoveCommand(*mMapView, DirectionNorthEast, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_RIGHT] = new MoveCommand(*mMapView, DirectionNorthEast, &mMoveCommandScalar);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][NAS2D::EventHandler::KeyCode::KEY_F1] = new SignalCommand(reportsUiSignal);
	mKeyModifierMap[NAS2D::EventHandler::KeyModifier::Shift | NAS2D::EventHandler::KeyModifier::Ctrl][NAS2D::EventHandler::KeyCode::KEY_F10] = new ShowCheatMenuCommand(cheatMenu, windowStack);
}


void KeyboardInputHandler::handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier keyModifiers)
{
	auto keyModifiersFitered = NAS2D::EventHandler::KeyModifier::None;
	mMoveCommandScalar = NAS2D::Utility<NAS2D::EventHandler>::get().shift(keyModifiers) ? 5 : 1;

	NAS2D::Utility<NAS2D::EventHandler>::get().shift(keyModifiers) ? keyModifiersFitered |= NAS2D::EventHandler::KeyModifier::Shift : keyModifiersFitered;

	NAS2D::Utility<NAS2D::EventHandler>::get().control(keyModifiers) ? keyModifiersFitered |= NAS2D::EventHandler::KeyModifier::Ctrl : keyModifiersFitered;

	if (mKeyModifierMap.find(keyModifiersFitered) != mKeyModifierMap.end() && mKeyModifierMap[keyModifiersFitered].find(key) != mKeyModifierMap[keyModifiersFitered].end())
	{
		mKeyModifierMap[keyModifiersFitered][key]->execute();
	}
	else if (mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None].find(key) != mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None].end())
	{
		mKeyModifierMap[NAS2D::EventHandler::KeyModifier::None][key]->execute();
	}
}
