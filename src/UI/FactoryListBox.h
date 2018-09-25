#pragma once

#include "NAS2D/NAS2D.h"

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
	typedef NAS2D::Signals::Signal1<Factory*> SelectionChangedCallback;

public:
	class FactoryListBoxItem : public ListBoxItem
	{
	public:
		FactoryListBoxItem(Factory* _f) : factory(_f) {}
		virtual ~FactoryListBoxItem() {}

	public:
		Factory* factory = nullptr;
		NAS2D::Point_2d	icon_slice;
	};

public:
	FactoryListBox();
	virtual ~FactoryListBox();

	void addItem(Factory* factory);
	void removeItem(Factory* factory);
	void currentSelection(Factory*);

	Factory* selectedFactory();

	virtual void update() final;

private:
	void _init();

private:
	typedef std::vector<FactoryListBoxItem> FactoryItemList;

private:
};
