#pragma once

#include "UI.h"

class Structure;

class StructureInspector : public Window
{
public:
	StructureInspector();
	~StructureInspector() override;

	void structure(Structure* s);
	Structure* structure() { return mStructure; }

	void check();

	void update() override;

protected:
	void init();

private:
	void btnCloseClicked();

	void drawPopulationRequirements();

private:
	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

private:
	Button btnClose;

	TextArea txtStateDescription;

	const NAS2D::Image& mIcons;

	std::string mStructureClass;

	Structure* mStructure = nullptr;
};
