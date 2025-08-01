#include "ComboBox.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/StringUtils.h>
#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>

#include <algorithm>


namespace
{
	const std::string EmptyString{};
	constexpr std::size_t MinimumDisplayItems{5};
}


ComboBox::ComboBox(SelectionChangedDelegate selectionChangedHandler) :
	ControlContainer{{&btnDown, &txtField, &lstItems}},
	btnDown{getImage("ui/icons/down.png")},
	lstItems{{this, &ComboBox::onListSelectionChange}},
	mMinHeight{std::max(txtField.size().y, btnDown.size().y)},
	mSelectionChangedHandler{selectionChangedHandler},
	mMaxDisplayItems{MinimumDisplayItems}
{
	btnDown.size({20, 20});

	txtField.editable(false);
	lstItems.visible(false);

	size(txtField.size() + NAS2D::Vector{btnDown.size().x, 0});

	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().connect({this, &ComboBox::onMouseDown});
}


ComboBox::~ComboBox()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseButtonDown().disconnect({this, &ComboBox::onMouseDown});
}


void ComboBox::onVisibilityChange(bool visible)
{
	ControlContainer::onVisibilityChange(visible);

	lstItems.visible(false);
}


void ComboBox::onResize()
{
	Control::onResize();

	// Enforce minimum size
	if (mRect.size.x < 50 || mRect.size.y < mMinHeight)
	{
		size({std::max(mRect.size.x, 50), std::max(mRect.size.y, mMinHeight)});
	}

	txtField.size(mRect.size - NAS2D::Vector{btnDown.size().x, 0});
	btnDown.position(txtField.area().crossXPoint());
	btnDown.height(mRect.size.y);
	lstItems.width(mRect.size.x);
	lstItems.position(area().crossYPoint());

	mBarRect = NAS2D::Rectangle{position(), {mRect.size.x, btnDown.size().y}};
}


void ComboBox::onMove(NAS2D::Vector<int> displacement)
{
	Control::onMove(displacement);

	txtField.position(position());
	btnDown.position(txtField.area().crossXPoint());
	lstItems.position(area().crossYPoint());

	mBarRect = NAS2D::Rectangle{position(), {mRect.size.x, btnDown.size().y}};
}


void ComboBox::onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position)
{
	if (!enabled() || !visible()) { return; }

	if (button != NAS2D::MouseButton::Left) { return; }

	if (mBarRect.contains(position))
	{
		onDropdownButtonClick();
	}
	else if (!lstItems.area().contains(position))
	{
		lstItems.visible(false);
		mRect = mBarRect;
	}
}


void ComboBox::onDropdownButtonClick()
{
	lstItems.visible(!lstItems.visible());
	if (lstItems.visible())
	{
		mRect.size.y += lstItems.size().y;
	}
	else
	{
		mRect = mBarRect;
	}
}


void ComboBox::clearSelected()
{
	lstItems.clearSelected();
	txtField.clear();
}


void ComboBox::onListSelectionChange()
{
	txtField.text(selectionText());
	lstItems.visible(false);
	mRect = mBarRect;
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}


std::size_t ComboBox::maxDisplayItems() const
{
	return mMaxDisplayItems;
}


/**
 * Sets the maximum number of items to display before showing a scroll bar.
 */
void ComboBox::maxDisplayItems(std::size_t count)
{
	mMaxDisplayItems = std::max(count, MinimumDisplayItems);
}


void ComboBox::addItem(const std::string& item, int tag)
{
	lstItems.add(item, tag);

	if (lstItems.count() > mMaxDisplayItems) { return; }
	lstItems.height(static_cast<int>(lstItems.count()) * lstItems.itemHeight() + 2);

	lstItems.clearSelected();
}


const std::string& ComboBox::selectionText() const
{
	return lstItems.isItemSelected() ? lstItems.selected().text : EmptyString;
}


int ComboBox::selectionUserData() const
{
	return lstItems.isItemSelected() ? lstItems.selected().userData : 0;
}


bool ComboBox::isItemSelected() const
{
	return lstItems.isItemSelected();
}


std::size_t ComboBox::selectedIndex() const
{
	return lstItems.selectedIndex();
}


void ComboBox::setSelected(std::size_t index) {
	lstItems.selectedIndex(index);
}


const std::string& ComboBox::text() const {
	return txtField.text();
}
