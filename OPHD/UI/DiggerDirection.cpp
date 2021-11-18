#include "DiggerDirection.h"
#include "../Map/Tile.h"


DiggerDirection::DiggerDirection() :
	Window{"Direction"},
	btnCancel{"Cancel"}
{
	position({0, 0});
	size({74, 170});

	// create a set of buttons
	add(btnDown, {5, 25});
	btnDown.image("ui/icons/arrow-down.png");
	btnDown.size({64, 34});
	btnDown.click().connect(this, &DiggerDirection::onDiggerDown);

	add(btnWest, {5, 68});
	btnWest.image("ui/icons/arrow-west.png");
	btnWest.size({32, 32});
	btnWest.click().connect(this, &DiggerDirection::onDiggerWest);

	add(btnNorth, {38, 68});
	btnNorth.image("ui/icons/arrow-north.png");
	btnNorth.size({32, 32});
	btnNorth.click().connect(this, &DiggerDirection::onDiggerNorth);

	add(btnSouth, {5, 101});
	btnSouth.image("ui/icons/arrow-south.png");
	btnSouth.size({32, 32});
	btnSouth.click().connect(this, &DiggerDirection::onDiggerSouth);

	add(btnEast, {38, 101});
	btnEast.image("ui/icons/arrow-east.png");
	btnEast.size({32, 32});
	btnEast.click().connect(this, &DiggerDirection::onDiggerEast);

	add(btnCancel, {5, 140});
	btnCancel.size({64, 25});
	btnCancel.click().connect(this, &DiggerDirection::onCancel);
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
