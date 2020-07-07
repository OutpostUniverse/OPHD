#pragma once

#include "NAS2D/Signal.h"
#include "NAS2D/Renderer/Point.h"

#include "Core/ListBoxBase.h"

#include "Core/UIContainer.h"
#include "Core/Slider.h"

#include "../Constants/UiConstants.h"
#include "../Things/Structures/Factory.h"

#include <string>
#include <vector>


/**
 * Implements a ListBox control.
 */
class FactoryListBox : public ListBoxBase
{
public:
	using SelectionChangedCallback = NAS2D::Signals::Signal<Factory*>;

public:
	class FactoryListBoxItem : public ListBoxItem
	{
	public:
		FactoryListBoxItem(Factory* newFactory) : factory(newFactory) {}

	public:
		Factory* factory = nullptr;
		NAS2D::Point<int> icon_slice;
	};

public:
	FactoryListBox();
	~FactoryListBox() override;

	void addItem(Factory* factory);
	void removeItem(Factory* factory);
	void currentSelection(Factory*);
	using ListBoxBase::currentSelection;

	Factory* selectedFactory();

	void update() override;

private:
	void _init();

private:
	using FactoryItemList = std::vector<FactoryListBoxItem>;
};
