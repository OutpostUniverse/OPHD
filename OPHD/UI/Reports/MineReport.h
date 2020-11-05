#pragma once

#include "ReportInterface.h"

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
#include "../Core/TextArea.h"
#include "../StructureListBox.h"
#include "../../Common.h"

#include <NAS2D/Renderer/Rectangle.h>


namespace NAS2D
{
	class Font;
	class Image;
}


class MineFacility;


class MineReport : public ReportInterface
{
public:
	MineReport();

	void selectStructure(Structure*) override;
	void refresh() override;

	void fillLists() override;
	void clearSelection() override;

	void update() override;

private:
	void btnShowAllClicked();
	void btnShowActiveClicked();
	void btnShowIdleClicked();
	void btnShowTappedOutClicked();
	void btnShowDisabledClicked();

	void btnIdleClicked();
	void btnDigNewLevelClicked();
	void btnTakeMeThereClicked();

	void btnAddTruckClicked();
	void btnRemoveTruckClicked();

	void filterButtonClicked();

	void lstMineFacilitySelectionChanged();

	void updateManagementButtonsVisiblity();

	void resized(Control*);

	void drawMineFacilityPane(const NAS2D::Point<int>&);
	void drawOreProductionPane(const NAS2D::Point<int>&);
	void drawTruckMangementPane(const NAS2D::Point<int>&);
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

	StructureListBox lstMineFacilities;

	Structure* selectedFacility = nullptr;

	int mAvailableTrucks = 0;
};
