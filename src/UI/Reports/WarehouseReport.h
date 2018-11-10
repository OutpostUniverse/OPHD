#pragma once

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
#include "../Core/UIContainer.h"
#include "../Core/TextArea.h"
#include "../FactoryListBox.h"


class WarehouseReport : public UIContainer
{
public:

public:
	WarehouseReport();
	virtual ~WarehouseReport();

	virtual void update() final;

private:
	void init();

private:
};
