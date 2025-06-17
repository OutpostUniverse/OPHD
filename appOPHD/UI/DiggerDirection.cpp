#include "DiggerDirection.h"

#include <stdexcept>


DiggerDirection::DiggerDirection(DirectionSelectedDelegate directionSelectedHandler) :
	Window{"Direction"},
	btnDown{getImage("ui/icons/arrow-down.png"), {this, &DiggerDirection::onDiggerDown}},
	btnNorth{getImage("ui/icons/arrow-north.png"), {this, &DiggerDirection::onDiggerNorth}},
	btnEast{getImage("ui/icons/arrow-east.png"), {this, &DiggerDirection::onDiggerEast}},
	btnSouth{getImage("ui/icons/arrow-south.png"), {this, &DiggerDirection::onDiggerSouth}},
	btnWest{getImage("ui/icons/arrow-west.png"), {this, &DiggerDirection::onDiggerWest}},
	btnCancel{"Cancel", {this, &DiggerDirection::onCancel}},
	mDirectionSelectedHandler{directionSelectedHandler}
{
	position({0, 0});
	size({74, 170});

	btnDown.size({64, 34});

	btnWest.size({32, 32});
	btnNorth.size({32, 32});
	btnSouth.size({32, 32});
	btnEast.size({32, 32});

	btnCancel.size({64, 25});

	add(btnDown, {5, 25});

	add(btnWest, {5, 68});
	add(btnNorth, {38, 68});
	add(btnSouth, {5, 101});
	add(btnEast, {38, 101});

	add(btnCancel, {5, 140});

	if (directionSelectedHandler.empty())
	{
		throw std::runtime_error("DiggerDirection needs a non-empty directionSelectedHandler");
	}
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
	mDirectionSelectedHandler(Direction::Down, *mTile);
}


void DiggerDirection::onDiggerNorth()
{
	mDirectionSelectedHandler(Direction::North, *mTile);
}


void DiggerDirection::onDiggerSouth()
{
	mDirectionSelectedHandler(Direction::South, *mTile);
}


void DiggerDirection::onDiggerEast()
{
	mDirectionSelectedHandler(Direction::East, *mTile);
}


void DiggerDirection::onDiggerWest()
{
	mDirectionSelectedHandler(Direction::West, *mTile);
}
