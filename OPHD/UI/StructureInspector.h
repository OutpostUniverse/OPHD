#pragma once

#include "Core/Window.h"
#include "Core/Button.h"


class Structure;


class StructureInspector : public Window
{
public:
	StructureInspector();
	~StructureInspector() override;

	void structure(Structure* structure);
	Structure* structure() { return mStructure; }

	void update() override;

protected:
	void init();

private:
	void btnCloseClicked();
	std::string getDisabledReason() const;
	void drawPopulationRequirements();

private:
	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

private:
	Button btnClose;
	const NAS2D::Image& mIcons;
	Structure* mStructure = nullptr;
};
