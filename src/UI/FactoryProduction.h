#pragma once

#include "UI.h"
#include "IconGrid.h"

#include "../Things/Structures/Factory.h"

class FactoryProduction : public UIContainer
{
public:

	FactoryProduction(Font& font);
	virtual ~FactoryProduction();

	void factory(Factory* _f);

	virtual void update();

	virtual void hide();

protected:

	virtual void init();

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseUp(MouseButton button, int x, int y);

private:

	void btnOkayClicked();
	void btnCancelClicked();

	FactoryProduction();
	FactoryProduction(const FactoryProduction&);
	FactoryProduction& operator=(const FactoryProduction&);

	void productionSelectionChanged(const std::string&);

	Factory*					mFactory;

	Factory::ProductionType		mProductionType;

	IconGrid					mProductionGrid;

	Button						btnIdle;
	Button						btnOkay;
	Button						btnCancel;
};