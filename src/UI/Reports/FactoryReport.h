#pragma once

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
#include "../Core/UIContainer.h"


class FactoryReport : public UIContainer
{
public:
	FactoryReport();
	virtual ~FactoryReport();

	virtual void update() final;
private:
	void init();

private:
	Button			btnButton1;

	ComboBox		cboFilterByProduct;
};