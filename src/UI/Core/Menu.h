#ifndef __MENU__
#define __MENU__

#include "NAS2D/NAS2D.h"

#include <map>
#include <string>
#include <algorithm>

#include "Control.h"

class Menu: public Control
{
public:

	typedef Gallant::Signal0<void> SelectionChangedCallback;

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

	void onMouseDown(MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int relX, int relY);

	const Rectangle_2d& rect() const { return mRect; }

	int currentSelection() const { return mCurrentSelection; }
	void currentSelection(int selection) { mCurrentSelection = selection; mSelectionChanged(); }

	const std::string& selectionText() const { return mItems[mCurrentSelection]; }

	void width(int w) { mRect.w(w); }

	void update();

	bool empty() const;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

private:

	Font& font() { return *mFont; }

	int							mCurrentHighlight;	/**< Currently highlighted selection index. */
	int							mCurrentSelection;	/**< Current selection index. */

	Font*						mFont;				/**< Internal font to use for the menu. */

	StringList					mItems;				/**< List of items preserved in the order in which they're added. */

	Color_4ub					mText;				/**< Text Color */
	Color_4ub					mHighlightBg;		/**< Highlight Background color. */
	Color_4ub					mHighlightText;		/**< Text Color for an item that is currently highlighted. */

	Rectangle_2d				mRect;				/**< Internal rectangle area. */

	SelectionChangedCallback	mSelectionChanged;	/**< Callback for selection changed callback. */

	bool						mSorted;			/**< Flag indicating that all Items should be sorted. */
};

#endif