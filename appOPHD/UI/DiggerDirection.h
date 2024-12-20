#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>
#include "../Common.h"


class Tile;


class DiggerDirection : public Window
{
public:
	using Signal = NAS2D::Signal<Direction, Tile&>;

	DiggerDirection();

	void update() override;

	Signal::Source& directionSelected() { return mSignal; }

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

	Signal mSignal;

	Tile* mTile = nullptr;
};
