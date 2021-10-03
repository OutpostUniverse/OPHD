#pragma once

#include "Core/Window.h"
#include "Core/Button.h"

#include "StringTable.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Point.h>


class Structure;


class StructureInspector : public Window
{
public:
	StructureInspector();

	void structure(Structure* structure);
	Structure* structure() { return mStructure; }

	void update() override;

private:
	void onClose();
	std::string getDisabledReason() const;
	void drawStructureSpecificTable(NAS2D::Point<int> position, NAS2D::Renderer& renderer);
	std::string formatAge() const;

	StringTable buildStringTable() const;

	Button btnClose;
	const NAS2D::Image& mIcons;
	Structure* mStructure = nullptr;
};
