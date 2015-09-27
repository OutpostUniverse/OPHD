#ifndef __THINGY_EDIT_STATE__
#define __THINGY_EDIT_STATE__


#include "NAS2D/NAS2D.h"

#include "../ui/Button.h"
#include "../ui/Menu.h"
#include "../ui/TextField.h"

#include "../ThingCatalog.h"


class EditState: public State
{
public:
	EditState();
	~EditState();

protected:
	void initialize();
	State* update();

private:

	void onMouseMove(int x, int y, int relX, int relY);
	void onMouseDown(MouseButton button, int x, int y);

	void onKeyDown(KeyCode key, KeyModifier mod);

	void catagoryMenuChanged();
	void thingMenuChanged();

	Font			mFont;

	Menu			mMenuCatagories;
	Menu			mMenuThings;

	TextField		mTxtCatagoryName;

	Button			mBtnCatagoryAdd;
	Button			mBtnCatagoryDelete;

	Image			mMousePointer;
	Point_2d		mMousePosition;

	ThingCatalog	mThingCatalog;
};


#endif