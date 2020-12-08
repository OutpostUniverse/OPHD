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

	void warehouse(Warehouse* w);

	void hide() override;
	void update() override;

private:
	void btnCloseClicked();

	Warehouse* mWarehouse = nullptr;
	Button btnClose;
};
