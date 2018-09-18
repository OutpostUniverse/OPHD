#pragma once

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
//#include "../Core/ProgressBar.h"
#include "../Core/UIContainer.h"
#include "../Core/TextArea.h"
#include "../FactoryListBox.h"


class FactoryReport : public UIContainer
{
public:
	FactoryReport();
	virtual ~FactoryReport();

	void factoryList(StructureList&);

	virtual void update() final;

private:
	void init();

	void fillFactoryList();
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

	void lstFactoryListSelectionChanged(Factory*);
	void lstProductsSelectionChanged();
	void cboFilterByProductSelectionChanged();

	void checkFactoryActionControls();

	void resized(Control*);

	void drawDetailPane(Renderer& r);
	void drawProductPane(Renderer& r);

	virtual void visibilityChanged(bool visible) final;

private:
	Button			btnShowAll;
	Button			btnShowSurface;
	Button			btnShowUnderground;
	Button			btnShowActive;
	Button			btnShowIdle;
	Button			btnShowDisabled;

	Button			btnIdle;
	Button			btnClearProduction;
	Button			btnTakeMeThere;

	Button			btnApply;

	ComboBox		cboFilterByProduct;

	FactoryListBox	lstFactoryList;

	ListBox			lstProducts;

	TextArea		txtProductDescription;

	StructureList	mFactories;
};
