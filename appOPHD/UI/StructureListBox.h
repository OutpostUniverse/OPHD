#pragma once

#include <libControls/ListBoxBase.h>

#include "../Common.h"

#include <NAS2D/Signal/Signal.h>


class Structure;


/**
 * Implements a ListBox control.
 */
class StructureListBox : public ListBoxBase
{
public:
	using SelectionChangedSignal = NAS2D::Signal<Structure*>;

	struct StructureListBoxItem : public ListBoxItem
	{
		StructureListBoxItem(Structure* s);

		Structure* structure = nullptr; /**< Pointer to a Structure. */
		std::string structureState; /**< String description of the state of a Structure. */
		StructureState colorIndex; /**< Index to use from the listbox color table. */
	};


	StructureListBox();

	void addItem(Structure*);
	void setSelected(Structure*);

	Structure* selectedStructure();

	StructureListBoxItem* last();

	void update() override;
};
