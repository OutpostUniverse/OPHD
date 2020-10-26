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

	void btnApplyClicked();

	void filterButtonClicked();

	void lstMineFacilitySelectionChanged();

	void resized(Control*);

	void visibilityChanged(bool visible) override;

private:
	const NAS2D::Font& font;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;

	NAS2D::Rectangle<int> detailPanelRect;

	Button btnShowAll;
	Button btnShowActive;
	Button btnShowIdle;
	Button btnShowTappedOut;
	Button btnShowDisabled;

	Button btnApply;

	StructureListBox lstMineFacilities;

	Structure* selectedFacility = nullptr;
	ProductType selectedProductType = ProductType::PRODUCT_NONE;
};
