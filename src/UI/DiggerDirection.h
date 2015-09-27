#ifndef __DIGGER_DIRECTION__
#define __DIGGER_DIRECTION__

#include "UI.h"


class DiggerDirection: public UIContainer
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

	typedef Gallant::Signal1<DiggerSelection> Callback;

	DiggerDirection(Font& font);
	virtual ~DiggerDirection();

	virtual void update();

	Callback& directionSelected() { return mCallback; }

protected:

	virtual void init();

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseUp(MouseButton button, int x, int y);

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


	Button		btnDown;
	Button		btnNorth;
	Button		btnEast;
	Button		btnSouth;
	Button		btnWest;
	Button		btnCancel;

	Callback	mCallback;
};


#endif