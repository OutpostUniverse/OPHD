#pragma once

#include <libControls/Window.h>
#include <libControls/Button.h>


class Warehouse;


/**
* Implements a Factory Production dialog interface.
*/
class WarehouseInspector : public Window
{
public:
	WarehouseInspector();

	void warehouse(const Warehouse* warehouse);

	void hide() override;
	void drawClientArea(NAS2D::Renderer& renderer) const override;

private:
	void onClose();

	const Warehouse* mWarehouse = nullptr;
	Button btnClose;
};
