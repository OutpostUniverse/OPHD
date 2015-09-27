#ifndef __TUBES_PALETTE__
#define __TUBES_PALETTE__

#include "UI.h"

#include "../Tile.h"

class TubesPalette: public UIContainer
{
public:

	typedef Gallant::Signal1<ConnectorDir> Callback;

	TubesPalette(Font& font);
	virtual ~TubesPalette();

	virtual void update();

	Callback& tubeTypeSelected() { return mCallback; }

protected:

	virtual void init();

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseUp(MouseButton button, int x, int y);

private:

	void btnCancelClicked();
	void btnTubesIntersectionClicked();
	void btnTubesRightClicked();
	void btnTubesLeftClicked();

	TubesPalette();
	TubesPalette(const TubesPalette&);
	TubesPalette& operator=(const TubesPalette&);


	Button		btnCancel;
	Button		btnTubesIntersection;
	Button		btnTubesRight;
	Button		btnTubesLeft;


	Callback	mCallback;
};


#endif