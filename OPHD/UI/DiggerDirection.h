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

	Callback::Source& directionSelected() { return mCallback; }

	void setParameters(Tile* tile);

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

	Callback mCallback;

	Tile* mTile = nullptr;
};
