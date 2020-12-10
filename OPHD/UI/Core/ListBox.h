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
		std::string text; /**< Text of the ListBoxItem. */
		int tag = 0; /**< User defined int data attached to the item. */

		bool operator==(const std::string& rhs) { return text == rhs; }
		bool operator<(const ListBoxItem& lhs) { return text < lhs.text; }
	};


	ListBox();
	~ListBox() override;

	void addItem(const std::string& item, int tag = 0);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();
	void sort();

	std::size_t count() const { return mItems.size(); }
	unsigned int lineHeight() const { return mLineHeight; }

	std::size_t selectedIndex() const { return mSelectedIndex; }
	void currentSelection(std::size_t selection) { mSelectedIndex = selection; mSelectionChanged(); }
	void setSelectionByName(const std::string& item);
	void clearSelection() { mSelectedIndex = constants::NO_SELECTION; }

	bool isItemSelected() const;
	const ListBoxItem& selected() const;

	std::size_t currentHighlight() const { return mHighlightIndex; }

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

	std::size_t mHighlightIndex = constants::NO_SELECTION;
	std::size_t mSelectedIndex = 0;
	std::size_t mScrollOffsetInPixels = 0;

	unsigned int mLineHeight = 0; /**< Height of an item line. */

	std::vector<ListBoxItem> mItems; /**< List of items preserved in the order in which they're added. */

	NAS2D::Rectangle<int> mScrollArea;

	NAS2D::Color mBorderColorNormal = NAS2D::Color{75, 75, 75};
	NAS2D::Color mBorderColorActive = NAS2D::Color{0, 185, 0};

	NAS2D::Color mBackgroundColorNormal = NAS2D::Color{0, 85, 0, 220};
	NAS2D::Color mBackgroundColorMouseHover = NAS2D::Color::DarkGreen;
	NAS2D::Color mBackgroundColorSelected = NAS2D::Color::DarkGreen.alphaFade(80);

	NAS2D::Color mTextColorNormal = NAS2D::Color::White;
	NAS2D::Color mTextColorMouseHover = NAS2D::Color::White;

	SelectionChangedCallback mSelectionChanged; /**< Callback for selection changed callback. */
	Slider mSlider;
};
