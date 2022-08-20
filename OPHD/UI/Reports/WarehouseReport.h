#pragma once

#include "ReportInterface.h"

#include "../ProductListBox.h"
#include "../StructureListBox.h"

#include "../Core/Button.h"

#include <NAS2D/Math/Point.h>

#include <vector>


namespace NAS2D
{
	class Font;
	class Image;
}

class Warehouse;
class Structure;


class WarehouseReport : public ReportInterface
{
public:
	WarehouseReport();
	~WarehouseReport() override;

	void fillLists() override;
	void clearSelected() override;

	void refresh() override;
	void selectStructure(Structure*) override;

	void update() override;

private:
	void computeTotalWarehouseCapacity();

	void fillListFromStructureList(const std::vector<Warehouse*>&);

	void onResize() override;

	void onShowAll();
	void onSpaceAvailable();
	void onFull();
	void onEmpty();
	void onDisabled();

	void onTakeMeThere();

	void fillListSpaceAvailable();
	void fillListFull();
	void fillListEmpty();
	void fillListDisabled();

	void onDoubleClick(NAS2D::EventHandler::MouseButton, NAS2D::Point<int> position);

	void onStructureSelectionChange();

	void filterButtonClicked();

	void drawLeftPanel(NAS2D::Renderer&);
	void drawRightPanel(NAS2D::Renderer&);

	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;
	const NAS2D::Image& imageWarehouse;

	Warehouse* selectedWarehouse = nullptr;

	Button btnShowAll;
	Button btnSpaceAvailable;
	Button btnFull;
	Button btnEmpty;
	Button btnDisabled;

	Button btnTakeMeThere;

	StructureListBox lstStructures;
	ProductListBox lstProducts;

	std::size_t warehouseCount;
	int warehouseCapacityTotal;
	float warehouseCapacityPercent = 0.0f;
};
