#pragma once

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Renderer/Color.h"

#include <map>
#include <string>
#include <algorithm>

#include "UIContainer.h"
#include "Slider.h"

#include "../../Constants/UiConstants.h"

/**
 * Implements a ListBox control.
 */
class ListBox: public UIContainer
{
public:
	typedef NAS2D::Signals::Signal<> SelectionChangedCallback;

	struct ListBoxItem
	{
		ListBoxItem(const std::string& text, int tag) : Text(text), Tag(tag) {}

		bool operator==(const std::string& rhs) { return Text == rhs; }
		bool operator<(const ListBoxItem& lhs) { return Text < lhs.Text; }

		std::string		Text;			/**< Text of the ListBoxItem. */
		int				Tag = 0;		/**< User defined int data attached to the item. */
	};

	using ListBoxItems = std::vector<ListBoxItem>;

public:
	ListBox();
	virtual ~ListBox();
	
	void sorted(bool isSorted) { mSorted = isSorted; }
	bool sorted(bool) const { return mSorted; }

	void sort() { if (mSorted) { std::sort(mItems.begin(), mItems.end()); } }

	void textColor(const NAS2D::Color& color)	{ mText = color; }
	void selectColor(const NAS2D::Color& color)	{ mHighlightBg = color; }

	void addItem(const std::string& item, int tag = 0);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();

	size_t count() const { return mItems.size(); }
	unsigned int lineHeight() const { return mLineHeight; }

	void setSelectionByName(const std::string& item);

	unsigned int currentSelection() const { return mCurrentSelection; }
	void currentSelection(int selection) { mCurrentSelection = selection; mSelectionChanged(); }
	void clearSelection() { mCurrentSelection = constants::NO_SELECTION; }

	unsigned int currentHighlight() const { return mCurrentHighlight; }

	const std::string& selectionText() const;
	int selectionTag() const;

	void update();

	bool empty() const;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);
	void onMouseWheel(int x, int y);
	virtual void slideChanged(float newPosition);

	virtual void visibilityChanged(bool visible) final;

private:
	virtual void onSizeChanged() final;

	void _updateItemDisplay();
	void _init();

private:
	unsigned int				mCurrentHighlight = constants::NO_SELECTION;	/**< Currently highlighted selection index. */
	unsigned int				mCurrentSelection = 0;							/**< Current selection index. */
	unsigned int				mCurrentOffset = 0;								/**< Current selection index. */

	unsigned int				mItemWidth = 0;									/**< Width of items. */
	unsigned int				mLineHeight = 0;								/**< Height of an item line. */
	unsigned int				mLineCount = 0;									/**< Number of lines that can be displayed. */

	ListBoxItems				mItems;											/**< List of items preserved in the order in which they're added. */

	NAS2D::Color				mText = NAS2D::Color::White;					/**< Text Color */
	NAS2D::Color				mHighlightBg = NAS2D::Color::Green;				/**< Highlight Background color. */
	NAS2D::Color				mHighlightText = NAS2D::Color::White;			/**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback	mSelectionChanged;								/**< Callback for selection changed callback. */
	Slider						mSlider;										/**<  */
	
	bool						mSorted = false;								/**< Flag indicating that all Items should be sorted. */
};
