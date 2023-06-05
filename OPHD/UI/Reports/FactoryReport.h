#pragma once

#include "ReportInterface.h"

#include <libControls/Button.h>
#include <libControls/ComboBox.h>
#include <libControls/TextArea.h>
#include "../FactoryListBox.h"
#include "../../Common.h"
#include "../../ProductCatalogue.h"

#include <NAS2D/Math/Rectangle.h>


namespace NAS2D
{
	class Font;
	class Image;
}

class Factory;


class FactoryReport : public ReportInterface
{
public:
	FactoryReport();

	void selectStructure(Structure*) override;
	void refresh() override;

	void fillLists() override;
	void clearSelected() override;

	void update() override;

private:
	void fillFactoryList(ProductType);
	void fillFactoryList(bool);
	void fillFactoryList(StructureState);

	void onShowAll();
	void onShowSurface();
	void onShowUnderground();
	void onShowActive();
	void onShowIdle();
	void onShowDisabled();

	void onIdle();
	void onClearProduction();
	void onTakeMeThere();

	void onApply();

	void filterButtonClicked(bool clearCbo);

	void onListSelectionChange();
	void onProductSelectionChange();
	void onProductFilterSelectionChange();

	void checkFactoryActionControls();

	void onResize() override;

	void drawDetailPane(NAS2D::Renderer&);
	void drawProductPane(NAS2D::Renderer&);

	void onVisibilityChange(bool visible) override;

	const NAS2D::Font& font;
	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;
	const NAS2D::Image& factorySeed;
	const NAS2D::Image& factoryAboveGround;
	const NAS2D::Image& factoryUnderGround;
	const NAS2D::Image* factoryImage = nullptr;

	NAS2D::Rectangle<int> detailPanelRect;

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

	ListBox<> lstProducts;

	TextArea txtProductDescription;

	Factory* selectedFactory = nullptr;
	ProductType selectedProductType = ProductType::PRODUCT_NONE;
};
