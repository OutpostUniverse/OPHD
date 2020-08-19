#pragma once

#include "ReportInterface.h"

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
#include "../Core/TextArea.h"
#include "../FactoryListBox.h"
#include "../../Common.h"


namespace NAS2D {
	class Font;
	class Image;
}

class Factory;


class FactoryReport : public ReportInterface
{
public:
	FactoryReport();
	~FactoryReport() override;

	void selectStructure(Structure*) override;
	void refresh() override;

	void fillLists() override;
	void clearSelection() override;

	void update() override;

private:
	void fillFactoryList(ProductType);
	void fillFactoryList(bool);
	void fillFactoryList(StructureState);

	void btnShowAllClicked();
	void btnShowSurfaceClicked();
	void btnShowUndergroundClicked();
	void btnShowActiveClicked();
	void btnShowIdleClicked();
	void btnShowDisabledClicked();

	void btnIdleClicked();
	void btnClearProductionClicked();
	void btnTakeMeThereClicked();

	void btnApplyClicked();

	void filterButtonClicked(bool clearCbo);

	void lstFactoryListSelectionChanged();
	void lstProductsSelectionChanged();
	void cboFilterByProductSelectionChanged();

	void checkFactoryActionControls();

	void resized(Control*);

	void drawDetailPane(NAS2D::Renderer&);
	void drawProductPane(NAS2D::Renderer&);

	void visibilityChanged(bool visible) override;

private:
	const NAS2D::Font& font;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;
	const NAS2D::Image& factorySeed;
	const NAS2D::Image& factoryAboveGround;
	const NAS2D::Image& factoryUnderGround;
	const NAS2D::Image* factoryImage = nullptr;

	Button btnShowAll;
	Button btnShowSurface;
	Button btnShowUnderground;
	Button btnShowActive;
	Button btnShowIdle;
	Button btnShowDisabled;

	Button btnIdle;
	Button btnClearProduction;
	Button btnTakeMeThere;

	Button btnApply;

	ComboBox cboFilterByProduct;

	FactoryListBox lstFactoryList;

	ListBox lstProducts;

	TextArea txtProductDescription;

	Factory* SELECTED_FACTORY = nullptr;
};
