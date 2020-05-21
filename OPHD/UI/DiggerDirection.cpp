// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "DiggerDirection.h"



DiggerDirection::DiggerDirection() :
	btnCancel{"Cancel"}
{
	text("Direction");
	init();
}


DiggerDirection::~DiggerDirection()
{}


void DiggerDirection::init()
{
	position(0, 0);
	size({74, 170});

	// create a set of buttons
	add(&btnDown, 5, 25);
	btnDown.image("ui/icons/arrow-down.png");
	btnDown.size({64, 34});
	btnDown.click().connect(this, &DiggerDirection::btnDiggerDownClicked);

	add(&btnWest, 5, 68);
	btnWest.image("ui/icons/arrow-west.png");
	btnWest.size({32, 32});
	btnWest.click().connect(this, &DiggerDirection::btnDiggerWestClicked);

	add(&btnNorth, 38, 68);
	btnNorth.image("ui/icons/arrow-north.png");
	btnNorth.size({32, 32});
	btnNorth.click().connect(this, &DiggerDirection::btnDiggerNorthClicked);

	add(&btnSouth, 5, 101);
	btnSouth.image("ui/icons/arrow-south.png");
	btnSouth.size({32, 32});
	btnSouth.click().connect(this, &DiggerDirection::btnDiggerSouthClicked);

	add(&btnEast, 38, 101);
	btnEast.image("ui/icons/arrow-east.png");
	btnEast.size({32, 32});
	btnEast.click().connect(this, &DiggerDirection::btnDiggerEastClicked);

	add(&btnCancel, 5, 140);
	btnCancel.size({64, 25});
	btnCancel.click().connect(this, &DiggerDirection::btnCancelClicked);
}


void DiggerDirection::setParameters(Tile* tile)
{
	mTile = tile;
}


void DiggerDirection::downOnlyEnabled()
{
	btnDown.enabled(true);

	btnWest.enabled(false);
	btnNorth.enabled(false);
	btnSouth.enabled(false);
	btnEast.enabled(false);
}


void DiggerDirection::cardinalOnlyEnabled()
{
	btnDown.enabled(false);

	btnWest.enabled(true);
	btnNorth.enabled(true);
	btnSouth.enabled(true);
	btnEast.enabled(true);
}


void DiggerDirection::allEnabled()
{
	btnDown.enabled(true);

	btnWest.enabled(true);
	btnNorth.enabled(true);
	btnSouth.enabled(true);
	btnEast.enabled(true);
}

void DiggerDirection::btnCancelClicked()
{
	visible(false);
}


void DiggerDirection::update()
{
	if (!visible() || !mTile) { return; }
	Window::update();
}


void DiggerDirection::selectDown()
{
	btnDiggerDownClicked();
}


void DiggerDirection::btnDiggerDownClicked()
{
	mCallback(Direction::DIR_DOWN, mTile);
}


void DiggerDirection::btnDiggerNorthClicked()
{
	mCallback(Direction::DIR_NORTH, mTile);
}


void DiggerDirection::btnDiggerSouthClicked()
{
	mCallback(Direction::DIR_SOUTH, mTile);
}


void DiggerDirection::btnDiggerEastClicked()
{
	mCallback(Direction::DIR_EAST, mTile);
}


void DiggerDirection::btnDiggerWestClicked()
{
	mCallback(Direction::DIR_WEST, mTile);
}
