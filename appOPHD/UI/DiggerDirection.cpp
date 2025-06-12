#include "DiggerDirection.h"
#include "../Map/Tile.h"


DiggerDirection::DiggerDirection() :
	Window{"Direction"},
	btnDown{getImage("ui/icons/arrow-down.png"), {this, &DiggerDirection::onDiggerDown}},
	btnNorth{getImage("ui/icons/arrow-north.png"), {this, &DiggerDirection::onDiggerNorth}},
	btnEast{getImage("ui/icons/arrow-east.png"), {this, &DiggerDirection::onDiggerEast}},
	btnSouth{getImage("ui/icons/arrow-south.png"), {this, &DiggerDirection::onDiggerSouth}},
	btnWest{getImage("ui/icons/arrow-west.png"), {this, &DiggerDirection::onDiggerWest}},
	btnCancel{"Cancel", {this, &DiggerDirection::onCancel}}
{
	position({0, 0});
	size({74, 170});

	// create a set of buttons
	add(btnDown, {5, 25});
	btnDown.size({64, 34});

	add(btnWest, {5, 68});
	btnWest.size({32, 32});

	add(btnNorth, {38, 68});
	btnNorth.size({32, 32});

	add(btnSouth, {5, 101});
	btnSouth.size({32, 32});

	add(btnEast, {38, 101});
	btnEast.size({32, 32});

	add(btnCancel, {5, 140});
	btnCancel.size({64, 25});
}


void DiggerDirection::setParameters(Tile& tile)
{
	mTile = &tile;
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

void DiggerDirection::onCancel()
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
	onDiggerDown();
}


void DiggerDirection::onDiggerDown()
{
	mSignal(Direction::Down, *mTile);
}


void DiggerDirection::onDiggerNorth()
{
	mSignal(Direction::North, *mTile);
}


void DiggerDirection::onDiggerSouth()
{
	mSignal(Direction::South, *mTile);
}


void DiggerDirection::onDiggerEast()
{
	mSignal(Direction::East, *mTile);
}


void DiggerDirection::onDiggerWest()
{
	mSignal(Direction::West, *mTile);
}
