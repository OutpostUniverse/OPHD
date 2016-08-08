#pragma once

#include "UI.h"

#include "../Tile.h"

class StructureInspector : public Window
{
public:

	StructureInspector(Font& font);
	virtual ~StructureInspector();

	virtual void update();

	void structure(Structure* _st) { mStructure = _st; }

protected:
	virtual void init();

private:
	StructureInspector();
	StructureInspector(const StructureInspector&);
	StructureInspector& operator=(const StructureInspector&);

	void btnCloseClicked();

	void drawResourcePool(const std::string& title, ResourcePool& rp, int x, int y);

private:
	Button		btnClose;

	Font		mBold;

	Image		mIcons;

	Structure*	mStructure;
};
