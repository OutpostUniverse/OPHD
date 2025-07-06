#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/Point.h>


class Structure;
class StringTable;


class StructureInspector : public Window
{
public:
	StructureInspector();

	void showStructure(Structure& structure);
	Structure* structure() { return mStructure; }

	void drawClientArea() const override;

protected:
	void onVisibilityChange(bool visible) override;
	void onClose();

	StringTable buildGenericStringTable() const;
	StringTable buildSpecificStringTable(NAS2D::Point<int> position) const;

private:
	Button btnClose;
	const NAS2D::Image& mIcons;
	Structure* mStructure = nullptr;
};
