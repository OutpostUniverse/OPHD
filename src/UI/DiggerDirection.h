#pragma once

#include "UI.h"

#include "../Map/Tile.h"

class DiggerDirection: public Window
{
public:
	/**
	 * 
	 */
	enum DiggerSelection
	{
		SEL_DOWN,
		SEL_NORTH,
		SEL_SOUTH,
		SEL_EAST,
		SEL_WEST
	};

	typedef NAS2D::Signals::Signal2<DiggerSelection, Tile*> Callback;

public:
	DiggerDirection();
	virtual ~DiggerDirection();

	virtual void update();

	Callback& directionSelected() { return mCallback; }

	void setParameters(Tile* tile);

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

private:
	DiggerDirection(const DiggerDirection&) = delete;				/**< Not allowed. */
	DiggerDirection& operator=(const DiggerDirection&) = delete;	/**< Not allowed. */

private:
	Button				btnDown;
	Button				btnNorth;
	Button				btnEast;
	Button				btnSouth;
	Button				btnWest;
	Button				btnCancel;

	Callback			mCallback;

	Tile*				mTile = nullptr;
};
