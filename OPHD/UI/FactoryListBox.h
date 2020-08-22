#pragma once

#include "Core/ListBoxBase.h"

#include <NAS2D/Signal.h>
#include <NAS2D/Renderer/Point.h>

#include <string>
#include <vector>


class Factory;


/**
 * Implements a ListBox control.
 */
class FactoryListBox : public ListBoxBase
{
public:
	using SelectionChangedCallback = NAS2D::Signals::Signal<Factory*>;

	struct FactoryListBoxItem : public ListBoxItem
	{
		FactoryListBoxItem(Factory* newFactory) : factory(newFactory) {}

		Factory* factory = nullptr;
		NAS2D::Point<int> icon_slice;
	};


	FactoryListBox();

	void addItem(Factory* factory);
	void removeItem(Factory* factory);
	void currentSelection(Factory*);
	using ListBoxBase::currentSelection;

	Factory* selectedFactory();

	void update() override;

private:
	void _init();
};
