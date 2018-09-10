#pragma once

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
#include "../Core/UIContainer.h"
#include "../FactoryListBox.h"


class FactoryReport : public UIContainer
{
public:
	FactoryReport();
	virtual ~FactoryReport();

	virtual void update() final;

private:
	void init();

	void btnShowAllClicked();
	void btnShowSurfaceClicked();
	void btnShowUndergroundClicked();
	void btnShowActiveClicked();
	void btnShowIdleClicked();
	void btnShowDisabledClicked();

	void btnIdleClicked();
	void btnClearProductionClicked();
	void btnTakeMeThereClicked();

	void filterButtonClicked();

	void lstFactoryListSelectionChanged(Factory*);

	void resized(Control*);

	void drawDetailPane(Renderer& r);
	void drawProductPane(Renderer& r);

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

	ComboBox		cboFilterByProduct;

	FactoryListBox	lstFactoryList;

	ListBox			lstProducts;
};