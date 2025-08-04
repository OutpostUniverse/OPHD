#pragma once

#include "Report.h"

#include "../StructureListBox.h"

#include <libControls/Button.h>

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Signal/Delegate.h>


namespace NAS2D
{
	class Font;
	class Image;
}

class StructureManager;
class MineFacility;


class MineReport : public Report
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;

	MineReport(const StructureManager& structureManager, TakeMeThereDelegate takeMeThereHandler);

	bool canView(const Structure& structure) override;
	void selectStructure(Structure& structure) override;
	void clearSelected() override;
	void fillLists() override;
	void refresh() override;

	void update() override;
	void draw(NAS2D::Renderer& renderer) const override;

protected:
	void onResize() override;
	void onVisibilityChange(bool visible) override;
	void onManagementButtonsVisibilityChange();

	void onFilterButtonClicked();
	void onShowAll();
	void onShowActive();
	void onShowIdle();
	void onShowExhausted();
	void onShowDisabled();

	void onMineFacilitySelectionChange();

	void onIdle();
	void onDigNewLevel();
	void onTakeMeThere();

	void onAddTruck();
	void onRemoveTruck();

	void drawMineFacilityPane(NAS2D::Renderer& renderer, const NAS2D::Point<int>& origin) const;
	void drawStatusPane(NAS2D::Renderer& renderer, const NAS2D::Point<int>& origin) const;
	void drawOreProductionPane(NAS2D::Renderer& renderer, const NAS2D::Point<int>& origin) const;

private:
	const StructureManager& mStructureManager;

	TakeMeThereDelegate mTakeMeThereHandler;
	const NAS2D::Font& font;
	const NAS2D::Font& fontBold;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	const NAS2D::Image& mineFacilityImage;
	const NAS2D::Image& uiIcons;

	Button btnShowAll;
	Button btnShowActive;
	Button btnShowIdle;
	Button btnShowExhausted;
	Button btnShowDisabled;

	Button btnIdle;
	Button btnDigNewLevel;
	Button btnTakeMeThere;

	Button btnAddTruck;
	Button btnRemoveTruck;

	StructureListBox lstMineFacilities;
	MineFacility* mSelectedFacility;
	int mAvailableTrucks;
};
