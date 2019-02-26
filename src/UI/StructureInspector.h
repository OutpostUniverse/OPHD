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

	void check();

	virtual void update() final;

protected:
	void init();

private:
	void btnCloseClicked();

	void drawPopulationRequirements();

private:
	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

private:
	Button			btnClose;

	TextArea		txtStateDescription;

	NAS2D::Image	mIcons;

	std::string		mStructureClass;

	Structure*		mStructure = nullptr;
};
