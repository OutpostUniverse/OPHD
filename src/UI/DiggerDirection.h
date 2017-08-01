#pragma once

#include "UI.h"

#include "../Map/Tile.h"

class DiggerDirection: public Window
{
public:

	enum DiggerSelection
	{
		SEL_DOWN,
		SEL_NORTH,
		SEL_SOUTH,
		SEL_EAST,
		SEL_WEST
	};

	typedef NAS2D::Signals::Signal2<DiggerSelection, Tile*> Callback;

	DiggerDirection(Font& font);
	virtual ~DiggerDirection();

	virtual void update();

	Callback& directionSelected() { return mCallback; }

	void setParameters(Tile* _t);

	void selectDown();

	void downOnlyEnabled();
	void cardinalOnlyEnabled();
	void allEnabled();

protected:

	virtual void init();

	void btnCancelClicked();

private:

	void btnDiggerDownClicked();
	void btnDiggerNorthClicked();
	void btnDiggerSouthClicked();
	void btnDiggerEastClicked();
	void btnDiggerWestClicked();

	DiggerDirection();
	DiggerDirection(const DiggerDirection&);
	DiggerDirection& operator=(const DiggerDirection&);


	Button				btnDown;
	Button				btnNorth;
	Button				btnEast;
	Button				btnSouth;
	Button				btnWest;
	Button				btnCancel;

	Callback			mCallback;

	Tile*				mTile;
};
