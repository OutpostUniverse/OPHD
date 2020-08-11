#pragma once

#include "Core/Window.h"
#include "Core/Button.h"


class Warehouse;

/**
* \brief Implements a Factory Production dialog interface.
*/
class WarehouseInspector : public Window
{
public:
	WarehouseInspector();
	~WarehouseInspector() override;

	void warehouse(Warehouse* w);

	void hide() override;
	void update() override;

protected:
	void init();

private:
	void btnCloseClicked();

private:
	WarehouseInspector(const WarehouseInspector&) = delete;
	WarehouseInspector& operator=(const WarehouseInspector&) = delete;

private:
	Warehouse* mWarehouse = nullptr;
	Button btnClose;
};
