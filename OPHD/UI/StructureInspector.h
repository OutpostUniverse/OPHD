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

	void drawLabelAndValue(NAS2D::Point<int> position, const std::string& title, const std::string& text);
	void drawPopulationRequirements();
	void drawStructureTypeSpecific();
	void drawResidenceText();

private:
	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

private:
	Button btnClose;

	TextArea txtStateDescription;

	NAS2D::Image mIcons;

	std::string mStructureClass;

	Structure* mStructure = nullptr;
};
