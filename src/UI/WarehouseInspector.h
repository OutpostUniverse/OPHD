#pragma once

#include "UI.h"
#include "IconGrid.h"

#include "../Things/Structures/Warehouse.h"


/**
* \brief Implements a Factory Production dialog interface.
*/
class WarehouseInspector : public Window
{
public:
	WarehouseInspector(Font& font);
	virtual ~WarehouseInspector();

	void warehouse(Warehouse* _wh);

	virtual void hide() final;
	virtual void update() final;

protected:
	virtual void init() final;

private:
	void btnCloseClicked();

private:
	WarehouseInspector() = delete;
	WarehouseInspector(const WarehouseInspector&) = delete;
	WarehouseInspector& operator=(const WarehouseInspector&) = delete;

private:
	Warehouse*			mWarehouse = nullptr;

	Font				mBold;

	Button				btnClose;
};
