#pragma once

#include "UI.h"

#include "../Tile.h"

class StructureInspector : public UIContainer
{
public:

	StructureInspector(Font& font);
	virtual ~StructureInspector();

	virtual void update();

	void structure(Structure* _st) { mStructure = _st; }

protected:

	virtual void init();

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseUp(MouseButton button, int x, int y);

private:

	void btnCloseClicked();

	StructureInspector();
	StructureInspector(const StructureInspector&);
	StructureInspector& operator=(const StructureInspector&);

	Button		btnClose;

	Font		mBold;

	Structure*	mStructure;
};
