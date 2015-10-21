#pragma once


#include "UI.h"

#include "../Things/Structures/Factory.h"

class FactoryProduction : public UIContainer
{
public:

	FactoryProduction(Font& font);
	virtual ~FactoryProduction();

	void factory(Factory* _f) { mFactory = _f; }

	virtual void update();

protected:

	virtual void init();

	virtual void onMouseDown(MouseButton button, int x, int y);
	virtual void onMouseUp(MouseButton button, int x, int y);

private:

	void btnIdleClicked();
	void btnOkayClicked();
	void btnApplyClicked();
	void btnCancelClicked();

	FactoryProduction();
	FactoryProduction(const FactoryProduction&);
	FactoryProduction& operator=(const FactoryProduction&);

	Factory*	mFactory;

	Menu		mnuProductionList;

	Button		btnIdle;
	Button		btnOkay;
	Button		btnCancel;
	Button		btnApply;
};