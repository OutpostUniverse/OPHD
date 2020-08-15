#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include "Core/TextArea.h"


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
	std::string getDisabledReason();

private:
	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

private:
	Button btnClose;

	const NAS2D::Image& mIcons;

	std::string mStructureClass;

	Structure* mStructure = nullptr;
};
