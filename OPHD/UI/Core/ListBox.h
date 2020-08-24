#pragma once

#include "Control.h"
#include "Slider.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Color.h>

#include <string>
#include <vector>
#include <cstddef>


namespace NAS2D {
	class Font;
}


/**
 * Implements a ListBox control.
 */
class ListBox: public Control
{
public:
	using SelectionChangedCallback = NAS2D::Signals::Signal<>;

	struct ListBoxItem
	{
		std::string Text; /**< Text of the ListBoxItem. */
		int Tag = 0; /**< User defined int data attached to the item. */

		bool operator==(const std::string& rhs) { return Text == rhs; }
		bool operator<(const ListBoxItem& lhs) { return Text < lhs.Text; }
	};

	using ListBoxItems = std::vector<ListBoxItem>;


	ListBox();
	~ListBox() override;

	void textColor(const NAS2D::Color& color) { mText = color; }
	void selectColor(const NAS2D::Color& color) { mHighlightBg = color; }

	void addItem(const std::string& item, int tag = 0);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();
	void sort();

	std::size_t count() const { return mItems.size(); }
	unsigned int lineHeight() const { return mLineHeight; }

	void setSelectionByName(const std::string& item);

	std::size_t currentSelection() const { return mCurrentSelection; }
	void currentSelection(std::size_t selection) { mCurrentSelection = selection; mSelectionChanged(); }
	void clearSelection() { mCurrentSelection = constants::NO_SELECTION; }

	std::size_t currentHighlight() const { return mCurrentHighlight; }

	const std::string& selectionText() const;
	int selectionTag() const;

	void update() override;

	bool empty() const;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);
	void onMouseWheel(int x, int y);
	virtual void slideChanged(float newPosition);

	void visibilityChanged(bool visible) override;

private:
	void onSizeChanged() override;

	void _updateItemDisplay();


	const NAS2D::Font& mFont;

	std::size_t mCurrentHighlight = constants::NO_SELECTION; /**< Currently highlighted selection index. */
	std::size_t mCurrentSelection = 0; /**< Current selection index. */
	std::size_t mCurrentOffset = 0; /**< Current selection index. */

	unsigned int mItemWidth = 0; /**< Width of items. */
	unsigned int mLineHeight = 0; /**< Height of an item line. */
	unsigned int mLineCount = 0; /**< Number of lines that can be displayed. */

	ListBoxItems mItems; /**< List of items preserved in the order in which they're added. */

	NAS2D::Color mText = NAS2D::Color::White; /**< Text Color */
	NAS2D::Color mHighlightBg = NAS2D::Color::DarkGreen; /**< Highlight Background color. */
	NAS2D::Color mHighlightText = NAS2D::Color::White; /**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback mSelectionChanged; /**< Callback for selection changed callback. */
	Slider mSlider;
};
