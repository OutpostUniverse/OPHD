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
	mMoveScalar = shiftMoveScalar;

	switch (key)
	{

	case NAS2D::EventHandler::KeyCode::KEY_w:
		mWKey_->execute();
		break;

	case NAS2D::EventHandler::KeyCode::KEY_UP:
		mUpKey_->execute();
		break;

	case NAS2D::EventHandler::KeyCode::KEY_s:
		mSKey_->execute();
		break;

	case NAS2D::EventHandler::KeyCode::KEY_DOWN:
		mDownKey_->execute();
		break;

	case NAS2D::EventHandler::KeyCode::KEY_a:
		mAKey_->execute();
		break;

	case NAS2D::EventHandler::KeyCode::KEY_LEFT:
		mLeftKey_->execute();
		break;

	case NAS2D::EventHandler::KeyCode::KEY_d:
		mDKey_->execute();
		break;

	case NAS2D::EventHandler::KeyCode::KEY_RIGHT:
		mRightKey_->execute();
		break;

	default:
		break;
	}
}
