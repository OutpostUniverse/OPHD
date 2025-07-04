#pragma once

#include <libOPHD/EnumDirection.h>

#include <libControls/Window.h>
#include <libControls/Button.h>

#include <NAS2D/Signal/Delegate.h>


class Tile;


class DiggerDirection : public Window
{
public:
	using DirectionSelectedDelegate = NAS2D::Delegate<void(Direction, Tile&)>;

	DiggerDirection(DirectionSelectedDelegate directionSelectedHandler);

	void setParameters(Tile& tile);

	void selectDown();

	void downOnlyEnabled();
	void cardinalOnlyEnabled();
	void allEnabled();

protected:
	void onCancel();

private:
	void onDiggerDown();
	void onDiggerNorth();
	void onDiggerSouth();
	void onDiggerEast();
	void onDiggerWest();

	Button btnDown;
	Button btnNorth;
	Button btnEast;
	Button btnSouth;
	Button btnWest;
	Button btnCancel;

	DirectionSelectedDelegate mDirectionSelectedHandler;

	Tile* mTile = nullptr;
};
