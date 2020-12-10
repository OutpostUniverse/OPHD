#pragma once

#include "Core/ListBoxBase.h"

#include "../Common.h"

#include <NAS2D/Signal.h>


class Structure;


/**
 * Implements a ListBox control.
 */
class StructureListBox : public ListBoxBase
{
public:
	using SelectionChangedCallback = NAS2D::Signals::Signal<Structure*>;

	struct StructureListBoxItem : public ListBoxItem
	{
		StructureListBoxItem(Structure* s);

		Structure* structure = nullptr; /**< Pointer to a Structure. */
		std::string structureState; /**< String description of the state of a Structure. */
		StructureState colorIndex; /**< Index to use from the listbox color table. */
	};


	StructureListBox();

	void addItem(Structure*);
	void removeItem(Structure*);
	void currentSelection(Structure*);

	Structure* selectedStructure();

	StructureListBoxItem* last();

	void update() override;
};
