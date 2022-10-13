#include "ComboBox.h"

#include "../../Constants/Strings.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Math/MathUtils.h>
#include <NAS2D/StringUtils.h>

#include <utility>
#include <algorithm>


ComboBox::ComboBox() :
	UIContainer{{&btnDown, &txtField, &lstItems}}
{
	auto& eventHandler = NAS2D::Utility<NAS2D::EventHandler>::get();
	eventHandler.mouseWheel().connect({this, &ComboBox::onMouseWheel});

	btnDown.image("ui/icons/down.png");
	btnDown.size({20, 20});

	txtField.editable(false);
	lstItems.visible(false);
	lstItems.height(300);

	lstItems.selectionChanged().connect({this, &ComboBox::onListSelectionChange});
}


ComboBox::~ComboBox()
{
	lstItems.selectionChanged().disconnect({this, &ComboBox::onListSelectionChange});
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
	if (mRect.width < 50 || mRect.height < 20)
	{
		size({std::max(mRect.width, 50), std::max(mRect.height, 20)});
	}

	txtField.size(size() - NAS2D::Vector{20, 0});
	btnDown.position(txtField.rect().crossXPoint());
	btnDown.height(mRect.height);
	lstItems.width(mRect.width);
	lstItems.position(rect().crossYPoint());

	mBarRect = NAS2D::Rectangle<int>::Create(position(), NAS2D::Vector{mRect.width, btnDown.size().y});
}


/**
 * Position changed event handler.
 */
void ComboBox::onMove(NAS2D::Vector<int> displacement)
{
	Control::onMove(displacement);

	txtField.position(position());
	btnDown.position(txtField.rect().crossXPoint());
	lstItems.position(rect().crossYPoint());

	mBarRect = NAS2D::Rectangle<int>::Create(position(), NAS2D::Vector{mRect.width, btnDown.size().y});
}


void ComboBox::onMouseDown(NAS2D::EventHandler::MouseButton button, NAS2D::Point<int> position)
{
	UIContainer::onMouseDown(button, position);

	if (!enabled() || !visible()) { return; }

	if (button != NAS2D::EventHandler::MouseButton::Left) { return; }

	if (mBarRect.contains(position))
	{
		lstItems.visible(!lstItems.visible());
		if (lstItems.visible())
		{
			mRect.height = mRect.height + lstItems.size().y;
		}
		else
		{
			mRect = mBarRect;
		}
	}
	else if (!lstItems.rect().contains(position))
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
	mSelectionChanged();
}


/**
 * Sets the maximum number of items to display before showing a scroll bar.
 */
void ComboBox::maxDisplayItems(std::size_t count)
{
	mMaxDisplayItems = count;

	if (count < constants::MinimumDisplayItems)
	{
		mMaxDisplayItems = constants::MinimumDisplayItems;
	}
}


/**
 * Adds an item to the list.
 */
void ComboBox::addItem(const std::string& item, int tag)
{
	lstItems.add(item, tag);

	if (lstItems.count() > mMaxDisplayItems) { return; }
	lstItems.height(static_cast<int>(lstItems.count() * lstItems.lineHeight()));

	lstItems.clearSelected();
}


/**
 * Gets the text of the selection.
 */
const std::string& ComboBox::selectionText() const
{
	return lstItems.isItemSelected() ? lstItems.selected().text : constants::EmptyString;
}


/**
 * Gets the tag value of the selected item.
 */
int ComboBox::selectionTag() const
{
	return lstItems.isItemSelected() ? lstItems.selected().tag : 0;
}


bool ComboBox::isItemSelected() const
{
	return lstItems.isItemSelected();
}


void ComboBox::setSelected(std::size_t index) {
	lstItems.setSelected(index);
	text(selectionText());
	mSelectionChanged();
}

void ComboBox::text(const std::string& text) {
	txtField.text(text);
	lstItems.selectIf([target = NAS2D::toLowercase(txtField.text())](const auto& item){ return NAS2D::toLowercase(item.text) == target; });
	mSelectionChanged();
}

const std::string& ComboBox::text() const {
	return txtField.text();
}
