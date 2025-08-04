#pragma once

#include "Report.h"

#include "../ProductListBox.h"
#include "../StructureListBox.h"

#include <libControls/Button.h>

#include <NAS2D/Math/Point.h>
#include <NAS2D/Signal/Delegate.h>

#include <vector>


namespace NAS2D
{
	class Font;
	class Image;
}

class Warehouse;
class Structure;
class StructureManager;


class WarehouseReport : public Report
{
public:
	using TakeMeThereDelegate = NAS2D::Delegate<void(const Structure*)>;

	WarehouseReport(const StructureManager& structureManager, TakeMeThereDelegate takeMeThereHandler);
	~WarehouseReport() override;

	bool canView(const Structure& structure) override;
	void selectStructure(Structure&) override;
	void clearSelected() override;
	void fillLists() override;
	void refresh() override;

	void update() override;
	void draw(NAS2D::Renderer& renderer) const override;

private:
	const Warehouse* selectedWarehouse() const;
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

	void drawLeftPanel(NAS2D::Renderer&) const;
	void drawRightPanel(NAS2D::Renderer&) const;

private:
	const StructureManager& mStructureManager;

	TakeMeThereDelegate mTakeMeThereHandler;
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
