#pragma once

#include "ReportInterface.h"

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
#include "../Core/UIContainer.h"
#include "../Core/TextArea.h"
#include "../FactoryListBox.h"


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
	void init();

	void fillFactoryList(ProductType);
	void fillFactoryList(bool);
	void fillFactoryList(Structure::StructureState);

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
};
