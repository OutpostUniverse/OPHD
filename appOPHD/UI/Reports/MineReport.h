#pragma once

#include "ReportInterface.h"

#include "../StructureListBox.h"

#include <libControls/Button.h>
#include <libControls/CheckBox.h>

#include <NAS2D/Math/Rectangle.h>

#include <array>


namespace NAS2D
{
	class Font;
	class Image;
}


class MineReport : public ReportInterface
{
public:
	MineReport();

	void selectStructure(Structure*) override;
	void refresh() override;

	void fillLists() override;
	void clearSelected() override;

	void update() override;

protected:
	void onShowAll();
	void onShowActive();
	void onShowIdle();
	void onShowTappedOut();
	void onShowDisabled();

	void onIdle();
	void onDigNewLevel();
	void onTakeMeThere();

	void onAddTruck();
	void onRemoveTruck();

	void onCheckBoxCommonMetalsChange();
	void onCheckBoxCommonMineralsChange();
	void onCheckBoxRareMetalsChange();
	void onCheckBoxRareMineralsChange();

	void onFilterButtonClicked();

	void onMineFacilitySelectionChange();

	void updateManagementButtonsVisibility();

	void onResize() override;
	void onVisibilityChange(bool visible) override;

	void drawMineFacilityPane(const NAS2D::Point<int>&);
	void drawOreProductionPane(const NAS2D::Point<int>&);
	void drawTruckManagementPane(const NAS2D::Point<int>&);
	void drawTruckHaulInfo(const NAS2D::Point<int>&);

private:
	const NAS2D::Font& font;
	const NAS2D::Font& fontBold;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	const NAS2D::Image& mineFacility;
	const NAS2D::Image& uiIcons;

	Button btnShowAll;
	Button btnShowActive;
	Button btnShowIdle;
	Button btnShowTappedOut;
	Button btnShowDisabled;

	Button btnIdle;
	Button btnDigNewLevel;
	Button btnTakeMeThere;

	Button btnAddTruck;
	Button btnRemoveTruck;

	std::array<CheckBox, 4> chkResources;

	StructureListBox lstMineFacilities;
	Structure* mSelectedFacility;
	int mAvailableTrucks;
};
