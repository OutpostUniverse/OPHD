#pragma once

#include "ReportInterface.h"

#include "../ProductListBox.h"
#include "../StructureListBox.h"

#include <libControls/Button.h>

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
	Warehouse* selectedWarehouse();
	void computeTotalWarehouseCapacity();

	void fillListFromStructureList(const std::vector<Warehouse*>&);

	void onResize() override;
	void onVisibilityChange(bool visible) override;
	void setVisibility();

	void onShowAll();
	void onFull();
	void onVacancy();
	void onEmpty();
	void onDisabled();

	void onTakeMeThere();

	void fillListFull();
	void fillListVacancy();
	void fillListEmpty();
	void fillListDisabled();

	void onDoubleClick(NAS2D::MouseButton, NAS2D::Point<int> position);

	void onStructureSelectionChange();

	void filterButtonClicked();

	void drawLeftPanel(NAS2D::Renderer&);
	void drawRightPanel(NAS2D::Renderer&);

	const NAS2D::Font& fontMedium;
	const NAS2D::Font& fontMediumBold;
	const NAS2D::Font& fontBigBold;
	const NAS2D::Image& imageWarehouse;

	Button btnShowAll;
	Button btnFull;
	Button btnVacancy;
	Button btnEmpty;
	Button btnDisabled;

	Button btnTakeMeThere;

	StructureListBox lstStructures;
	ProductListBox lstProducts;

	std::size_t warehouseCount;
	int warehouseCapacityTotal;
	int warehouseCapacityUsed;
};
