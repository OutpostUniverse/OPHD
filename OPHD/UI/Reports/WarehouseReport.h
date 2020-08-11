#pragma once

#include "ReportInterface.h"

#include "../ProductListBox.h"
#include "../StructureListBox.h"

#include "../Core/Button.h"

#include "../../Things/Structures/Structure.h"


class WarehouseReport : public ReportInterface
{
public:

public:
	WarehouseReport();
	~WarehouseReport() override;

	void fillLists() override;
	void clearSelection() override;

	void refresh() override;
	void selectStructure(Structure*) override;

	void update() override;

private:
	void init();
	void _resized(Control*);

	void _fillListFromStructureList(StructureList&);

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
	Button btnShowAll;
	Button btnSpaceAvailable;
	Button btnFull;
	Button btnEmpty;
	Button btnDisabled;

	Button btnTakeMeThere;

	StructureListBox lstStructures;
	ProductListBox lstProducts;
};
