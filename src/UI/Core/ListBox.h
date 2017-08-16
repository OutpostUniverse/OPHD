#pragma once

#include "NAS2D/NAS2D.h"

#include <map>
#include <string>
#include <algorithm>

#include "UIContainer.h"
#include "Slider.h"

class ListBox: public UIContainer
{
public:
	typedef NAS2D::Signals::Signal0<void> SelectionChangedCallback;

public:
	ListBox();
	~ListBox();

	void init();

	void sorted(bool _b) { mSorted = _b; }
	bool sorted(bool) const { return mSorted; }

	void sort() { std::sort(mItems.begin(), mItems.end()); }

	void textColor(const Color_4ub& color)	{ mText = color; }
	void selectColor(const Color_4ub& color)	{ mHighlightBg = color; }

	void addItem(const std::string& item);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);
	void dropAllItems();

	int currentSelection() const { return mCurrentSelection; }
	void currentSelection(int selection) { mCurrentSelection = selection; mSelectionChanged(); }

	int currentHighlight() const { return mCurrentHighlight; }

	const std::string& selectionText() const { return mItems[mCurrentSelection]; }

	void update();

	bool empty() const;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

protected:
	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);
	void onMouseWheel(int x, int y);
	virtual void slideChanged(double _position);

	virtual void visibilityChanged(bool visible) final;

private:
	virtual void onFontChanged() final;
	virtual void onSizeChanged() final;

	void _updateItemDisplay();

private:
	int							mCurrentHighlight = 0;	/**< Currently highlighted selection index. */
	int							mCurrentSelection = 0;	/**< Current selection index. */
	int							mCurrentOffset = 0;		/**< Current selection index. */

	int							mItemWidth = 0;			/**< Width of items. */
	int							mLineHeight = 0;		/**< Height of an item line. */
	int							mLineCount = 0;			/**< Number of lines that can be displayed. */

	StringList					mItems;					/**< List of items preserved in the order in which they're added. */

	Color_4ub					mText;					/**< Text Color */
	Color_4ub					mHighlightBg;			/**< Highlight Background color. */
	Color_4ub					mHighlightText;			/**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback	mSelectionChanged;		/**< Callback for selection changed callback. */
	Slider						mSlider;				/**<  */
	
	bool						mSorted;				/**< Flag indicating that all Items should be sorted. */
};
