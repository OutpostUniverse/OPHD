#pragma once

#include "ReportInterface.h"

#include "../ProductListBox.h"
#include "../StructureListBox.h"

#include "../Core/Button.h"

#include "../../Things/Structures/Structure.h"


namespace NAS2D {
	class Font;
	class Image;
}

class Warehouse;


class WarehouseReport : public ReportInterface
{
public:
	WarehouseReport();
	~WarehouseReport() override;

	void fillLists() override;
	void clearSelection() override;

	void refresh() override;
	void selectStructure(Structure*) override;

	void update() override;

private:
	void computeTotalWarehouseCapacity();

	void _fillListFromStructureList(const StructureList&);

	void _resized(Control*);

	void btnShowAllClicked();
	void btnSpaceAvailableClicked();
	void btnFullClicked();
	void btnEmptyClicked();
	void btnDisabledClicked();

	void btnTakeMeThereClicked();

	void fillListSpaceAvailable();
	void fillListFull();
	void fillListEmpty();
	void fillListDisabled();

	void doubleClicked(NAS2D::EventHandler::MouseButton, int, int);

	void lstStructuresSelectionChanged();

	void filterButtonClicked();

	void drawLeftPanel(NAS2D::Renderer&);
	void drawRightPanel(NAS2D::Renderer&);

private:
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
