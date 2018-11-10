#pragma once

#include "ReportInterface.h"

#include "../Core/Button.h"
#include "../Core/ComboBox.h"
#include "../Core/UIContainer.h"
#include "../Core/TextArea.h"
#include "../FactoryListBox.h"


class WarehouseReport : public ReportInterface
{
public:

public:
	WarehouseReport();
	virtual ~WarehouseReport();

	virtual void fillLists() final;
	virtual void clearSelection() final;

	virtual void update() final;

private:
	void init();

private:
};
