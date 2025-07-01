#include "ComboBox.h"

#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/StringUtils.h>

#include <algorithm>


namespace
{
	const std::string EmptyString{};
	constexpr std::size_t MinimumDisplayItems{5};
}


ComboBox::ComboBox(SelectionChangedDelegate selectionChangedHandler) :
	ControlContainer{{&btnDown, &txtField, &lstItems}},
	lstItems{{this, &ComboBox::onListSelectionChange}},
	mSelectionChangedHandler{selectionChangedHandler},
	mMaxDisplayItems{MinimumDisplayItems}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().connect({this, &ComboBox::onMouseWheel});

	btnDown.image("ui/icons/down.png");
	btnDown.size({20, 20});

	txtField.editable(false);
	lstItems.visible(false);

	size(txtField.size() + NAS2D::Vector{btnDown.size().x, 0});
}


ComboBox::~ComboBox()
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().disconnect({this, &ComboBox::onMouseWheel});
}


/**
 * Resized event handler.
 */
void ComboBox::onResize()
{
	Control::onResize();

	// Enforce minimum size
	if (mRect.size.x < 50 || mRect.size.y < 20)
	{
		size({std::max(mRect.size.x, 50), std::max(mRect.size.y, 20)});
	}

	txtField.size(size() - NAS2D::Vector{20, 0});
	btnDown.position(txtField.area().crossXPoint());
	btnDown.height(mRect.size.y);
	lstItems.width(mRect.size.x);
	lstItems.position(area().crossYPoint());

	mBarRect = NAS2D::Rectangle{position(), {mRect.size.x, btnDown.size().y}};
}


/**
 * Position changed event handler.
 */
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
	ControlContainer::onMouseDown(button, position);

	if (!enabled() || !visible()) { return; }

	if (button != NAS2D::MouseButton::Left) { return; }

	if (mBarRect.contains(position))
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
	else if (!lstItems.area().contains(position))
	{
		lstItems.visible(false);
		mRect = mBarRect;
	}
}


void ComboBox::onMouseWheel(NAS2D::Vector<int> /*scrollAmount*/)
{

}


void ComboBox::clearSelected()
{
	lstItems.clearSelected();
	txtField.clear();
}


/**
 * ListBox selection changed event handler.
 */
void ComboBox::onListSelectionChange()
{
	txtField.text(selectionText());
	lstItems.visible(false);
	mRect = mBarRect;
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}


/**
 * Sets the maximum number of items to display before showing a scroll bar.
 */
void ComboBox::maxDisplayItems(std::size_t count)
{
	mMaxDisplayItems = std::max(count, MinimumDisplayItems);
}


/**
 * Adds an item to the list.
 */
void ComboBox::addItem(const std::string& item, int tag)
{
	lstItems.add(item, tag);

	if (lstItems.count() > mMaxDisplayItems) { return; }
	lstItems.height(static_cast<int>(lstItems.count()) * lstItems.itemHeight() + 2);

	lstItems.clearSelected();
}


/**
 * Gets the text of the selection.
 */
const std::string& ComboBox::selectionText() const
{
	return lstItems.isItemSelected() ? lstItems.selected().text : EmptyString;
}


/**
 * Gets the tag value of the selected item.
 */
int ComboBox::selectionUserData() const
{
	return lstItems.isItemSelected() ? lstItems.selected().userData : 0;
}


bool ComboBox::isItemSelected() const
{
	return lstItems.isItemSelected();
}


void ComboBox::setSelected(std::size_t index) {
	lstItems.selectedIndex(index);
	text(selectionText());
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}

void ComboBox::text(const std::string& text) {
	txtField.text(text);
	lstItems.selectIf([target = NAS2D::toLowercase(txtField.text())](const auto& item){ return NAS2D::toLowercase(item.text) == target; });
	if (mSelectionChangedHandler) { mSelectionChangedHandler(); }
}

const std::string& ComboBox::text() const {
	return txtField.text();
}
