#pragma once

#include "Core/Window.h"
#include "Core/Button.h"
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Point.h>

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
	void drawStructureSpecificTable(NAS2D::Point<int> position, NAS2D::Renderer& renderer);
	std::string getDisabledReason();

	StructureInspector(const StructureInspector&) = delete;
	StructureInspector& operator=(const StructureInspector&) = delete;

	Button btnClose;
	const NAS2D::Image& mIcons;
	std::string mStructureClass;
	Structure* mStructure = nullptr;
};
