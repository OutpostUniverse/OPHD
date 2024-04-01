#include "KeyboardInputHandler.h"

#include <NAS2D/Math/Vector.h>
#include <OPHD/Map/MapOffset.h>
#include <OPHD/DirectionOffset.h>
#include <OPHD/States/MapViewState.h>


KeyboardInputHandler::KeyboardInputHandler(MapView& mapView) :
	mMapView(&mapView)
{
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_w] = new MoveCommand(*mMapView, DirectionNorthWest, &mKeyModifiers);
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_UP] =  new MoveCommand(*mMapView, DirectionNorthWest, &mKeyModifiers);
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_s] = new MoveCommand(*mMapView, DirectionSouthEast, &mKeyModifiers);
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_DOWN] = new MoveCommand(*mMapView, DirectionSouthEast, &mKeyModifiers);
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_a] = new MoveCommand(*mMapView, DirectionSouthWest, &mKeyModifiers);
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_LEFT] = new MoveCommand(*mMapView, DirectionSouthWest, &mKeyModifiers);
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_d] = new MoveCommand(*mMapView, DirectionNorthEast, &mKeyModifiers);
	mKeyCommandMap[NAS2D::EventHandler::KeyCode::KEY_RIGHT] = new MoveCommand(*mMapView, DirectionNorthEast, &mKeyModifiers);
}


void KeyboardInputHandler::handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier keyModifiers)

	switch (key)
	{
	mKeyModifiers = keyModifiers;

	if (mKeyCommandMap.find(key) != mKeyCommandMap.end())
	{
		mKeyCommandMap[key]->execute();
	}
}
