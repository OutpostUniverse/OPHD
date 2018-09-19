#pragma once

#include "NAS2D/NAS2D.h"

#include <map>
#include <string>

#include "Control.h"

#include "../../Constants.h"

class Menu: public Control
{
public:
	typedef NAS2D::Signals::Signal0<void> SelectionChangedCallback;

public:
	Menu();
	virtual ~Menu();

	void clear();
	bool empty() const;

	void sorted(bool _b) { mSorted = _b; }
	bool sorted(bool) const { return mSorted; }

	void sort();

	void textColor(const NAS2D::Color_4ub& color);
	void selectColor(const NAS2D::Color_4ub& color);

	void addItem(const std::string& item);
	void removeItem(const std::string& item);
	bool itemExists(const std::string& item);

	int currentSelection() const;
	void currentSelection(int selection);

	const std::string& selectionText() const;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

	void update();

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);

private:
	int							mCurrentHighlight = constants::NO_SELECTION;	/**< Currently highlighted selection index. */
	int							mCurrentSelection = constants::NO_SELECTION;	/**< Current selection index. */

	NAS2D::StringList			mItems;											/**< List of items preserved in the order in which they're added. */

	NAS2D::Color_4ub			mText = NAS2D::COLOR_WHITE;						/**< Text Color */
	NAS2D::Color_4ub			mHighlightBg = NAS2D::COLOR_GREEN;				/**< Highlight Background color. */
	NAS2D::Color_4ub			mHighlightText = NAS2D::COLOR_WHITE;			/**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback	mSelectionChanged;								/**< Callback for selection changed callback. */

	bool						mSorted = false;								/**< Flag indicating that all Items should be sorted. */
};
