#pragma once

#include "Report.h"

#include "../FactoryListBox.h"

#include <libOPHD/EnumProductType.h>

#include <libControls/Button.h>
#include <libControls/ComboBox.h>
#include <libControls/TextArea.h>

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Signal/Delegate.h>


namespace NAS2D
{
	class Font;
	class Image;
}

enum class StructureState;
class Factory;


class FactoryReport : public Report
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;

	FactoryReport(TakeMeThereDelegate takeMeThereHandler);

	bool canView(const Structure& structure) override;
	void selectStructure(Structure&) override;
	void clearSelected() override;
	void fillLists() override;
	void refresh() override;

	void update() override;
	void draw(NAS2D::Renderer& renderer) const override;

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

	void drawDetailPane(NAS2D::Renderer&) const;
	void drawProductPane(NAS2D::Renderer&) const;

	void onVisibilityChange(bool visible) override;

private:
	TakeMeThereDelegate mTakeMeThereHandler;
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

	TextArea mTxtProductDescription;

	Factory* selectedFactory = nullptr;
	ProductType selectedProductType = ProductType::PRODUCT_NONE;
};
