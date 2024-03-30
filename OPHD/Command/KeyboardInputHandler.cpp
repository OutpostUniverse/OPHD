#include "KeyboardInputHandler.h"

#include <NAS2D/Math/Vector.h>
#include <OPHD/Map/MapOffset.h>
#include <OPHD/DirectionOffset.h>
#include <OPHD/States/MapViewState.h>


KeyboardInputHandler::KeyboardInputHandler(MapView& mapView) :
	mMapView(&mapView)
{}


void KeyboardInputHandler::handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod){
	const int shiftMoveScalar = NAS2D::EventHandler::shift(mod) ? 5 : 1;

	switch (key)
	{

	case NAS2D::EventHandler::KeyCode::KEY_w:
	case NAS2D::EventHandler::KeyCode::KEY_UP:
		mMapView->moveView(MapOffset{DirectionNorthWest * shiftMoveScalar, 0});
		break;

	case NAS2D::EventHandler::KeyCode::KEY_s:
	case NAS2D::EventHandler::KeyCode::KEY_DOWN:
		mMapView->moveView(MapOffset{DirectionSouthEast * shiftMoveScalar, 0});
		break;

	case NAS2D::EventHandler::KeyCode::KEY_a:
	case NAS2D::EventHandler::KeyCode::KEY_LEFT:
		mMapView->moveView(MapOffset{DirectionSouthWest * shiftMoveScalar, 0});
		break;

	case NAS2D::EventHandler::KeyCode::KEY_d:
	case NAS2D::EventHandler::KeyCode::KEY_RIGHT:
		mMapView->moveView(MapOffset{DirectionNorthEast * shiftMoveScalar, 0});
		break;

	default:
		break;
	}
}
