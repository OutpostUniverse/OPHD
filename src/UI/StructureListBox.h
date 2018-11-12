#pragma once

#include "NAS2D/NAS2D.h"

#include "Core/ListBoxBase.h"

class Structure;

/**
 * Implements a ListBox control.
 */
class StructureListBox : public ListBoxBase
{
public:
	using SelectionChangedCallback = NAS2D::Signals::Signal1<Structure*>;

public:
	class StructureListBoxItem : public ListBoxItem
	{
	public:
		StructureListBoxItem(Structure* _st);
		virtual ~StructureListBoxItem();

	public:
		Structure* structure = nullptr;
		std::string structureState;
		size_t colorIndex = 0;
	};

public:
	StructureListBox();
	virtual ~StructureListBox();

	void addItem(Structure*);
	void removeItem(Structure*);
	void currentSelection(Structure*);

	Structure* selectedStructure();

	StructureListBoxItem* last();

	virtual void update() final;

private:
	void _init();

private:
	using StructureItemList = std::vector<StructureListBoxItem>;
};
