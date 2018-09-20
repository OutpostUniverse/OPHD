#pragma once

#include "UI.h"

#include "../Map/Tile.h"

class StructureInspector : public Window
{
public:
	StructureInspector();
	virtual ~StructureInspector();

	void structure(Structure* _st);
	Structure* structure() { return mStructure; }

	virtual void update() final;

protected:
	void init();

private:
	void btnCloseClicked();
	void drawResourcePool(const std::string& title, ResourcePool& rp, int x, int y);

private:
	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

private:
	Button			btnClose;

	NAS2D::Image	mIcons;

	std::string		mStructureClass;
	std::string		mStructureState;

	Structure*		mStructure = nullptr;
};
