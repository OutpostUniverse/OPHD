#pragma once

#include "UI.h"

#include "../Map/Tile.h"

class StructureInspector : public Window
{
public:

	StructureInspector(Font& font);
	virtual ~StructureInspector();

	virtual void update();

	void structure(Structure* _st) { mStructure = _st; }
	Structure* structure() { return mStructure; }

protected:
	virtual void init();

private:
	void btnCloseClicked();
	void drawResourcePool(const std::string& title, ResourcePool& rp, int x, int y);

private:
	StructureInspector() = delete;
	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

private:
	Button		btnClose;

	Font		mBold;

	Image		mIcons;

	Structure*	mStructure;
};
