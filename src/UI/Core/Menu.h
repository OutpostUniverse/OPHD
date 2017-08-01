#pragma once

#include "NAS2D/NAS2D.h"

#include <map>
#include <string>
#include <algorithm>

#include "Control.h"

class Menu: public Control
{
public:

	typedef NAS2D::Signals::Signal0<void> SelectionChangedCallback;

	Menu();
	~Menu();

	void font(Font& font) { mFont = &font; }

	void position(int x, int y);

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

	const std::string& selectionText() const { return mItems[mCurrentSelection]; }

	void update();

	bool empty() const;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

protected:

	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);

private:

	Font& font() { return *mFont; }

	int							mCurrentHighlight;	/**< Currently highlighted selection index. */
	int							mCurrentSelection;	/**< Current selection index. */

	Font*						mFont;				/**< Internal font to use for the menu. */

	StringList					mItems;				/**< List of items preserved in the order in which they're added. */

	Color_4ub					mText;				/**< Text Color */
	Color_4ub					mHighlightBg;		/**< Highlight Background color. */
	Color_4ub					mHighlightText;		/**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback	mSelectionChanged;	/**< Callback for selection changed callback. */

	bool						mSorted;			/**< Flag indicating that all Items should be sorted. */
};
