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

	void filterButtonClicked();

	void resized(Control*);

private:
	Button			btnShowAll;
	Button			btnShowSurface;
	Button			btnShowUnderground;
	Button			btnShowActive;
	Button			btnShowIdle;
	Button			btnShowDisabled;

	ComboBox		cboFilterByProduct;

	FactoryListBox	lstFactoryList;
};