#pragma once

#include "Report.h"

#include "../StructureListBox.h"

#include <libControls/Button.h>
#include <libControls/CheckBox.h>

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Signal/Delegate.h>

#include <array>


namespace NAS2D
{
	class Font;
	class Image;
}

class MineFacility;


class MineReport : public Report
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;

	MineReport(TakeMeThereDelegate takeMeThereHandler);

	bool canView(const Structure& structure) override;
	void selectStructure(Structure& structure) override;
	void clearSelected() override;
	void fillLists() override;
	void refresh() override;

	void update() override;
	void draw() const override;

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

	void onCheckBoxCommonMetalsChange();
	void onCheckBoxCommonMineralsChange();
	void onCheckBoxRareMetalsChange();
	void onCheckBoxRareMineralsChange();

	void onAddTruck();
	void onRemoveTruck();

	void drawMineFacilityPane(const NAS2D::Point<int>& origin) const;
	void drawStatusPane(const NAS2D::Point<int>& origin) const;
	void drawOreProductionPane(const NAS2D::Point<int>& origin) const;

private:
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

	std::array<CheckBox, 4> chkResources;

	StructureListBox lstMineFacilities;
	MineFacility* mSelectedFacility;
	int mAvailableTrucks;
};
