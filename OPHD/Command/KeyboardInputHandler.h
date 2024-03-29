#pragma once
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Utility.h>

#include "../Constants/Strings.h"
#include "../Map/MapView.h"
#include "../DirectionOffset.h"

#include "Command.h"
#include "Commands.cpp"

class KeyboardInputHandler
{
public:
	KeyboardInputHandler(MapView& mapView);
	void handleInput(NAS2D::EventHandler::KeyCode key, NAS2D::EventHandler::KeyModifier mod);

private:
	MapView* mMapView;
	int mMoveScalar = 1;
	Command* mUpKey_ = new MoveCommand(*mMapView, DirectionNorthWest, &mMoveScalar);
	Command* mWKey_ = mUpKey_;
	Command* mLeftKey_ = new MoveCommand(*mMapView, DirectionSouthWest, &mMoveScalar);
	Command* mAKey_ = mLeftKey_;
	Command* mRightKey_ = new MoveCommand(*mMapView, DirectionNorthEast, &mMoveScalar);
	Command* mDKey_ = mRightKey_;
	Command* mDownKey_ = new MoveCommand(*mMapView, DirectionSouthEast, &mMoveScalar);
	Command* mSKey_ = mDownKey_;
};
