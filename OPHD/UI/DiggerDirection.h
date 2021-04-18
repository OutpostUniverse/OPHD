#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "../Common.h"


class Tile;


class DiggerDirection: public Window
{
public:
	using Callback = NAS2D::Signal<Direction, Tile*>;

	DiggerDirection();

	void update() override;

	Callback& directionSelected() { return mCallback; }

	void setParameters(Tile* tile);

	void selectDown();

	void downOnlyEnabled();
	void cardinalOnlyEnabled();
	void allEnabled();

protected:
	void btnCancelClicked();

private:
	void btnDiggerDownClicked();
	void btnDiggerNorthClicked();
	void btnDiggerSouthClicked();
	void btnDiggerEastClicked();
	void btnDiggerWestClicked();

	Button btnDown;
	Button btnNorth;
	Button btnEast;
	Button btnSouth;
	Button btnWest;
	Button btnCancel;

	Callback mCallback;

	Tile* mTile = nullptr;
};
