#pragma once

#include "Control.h"
#include "Slider.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Renderer/Color.h>

#include <string>
#include <vector>
#include <utility>
#include <cstddef>


namespace NAS2D {
	class Font;
	class Renderer;
}


struct ListBoxItemText
{
	std::string text; /**< Text of the ListBoxItem. */
	int tag = 0; /**< User defined int data attached to the item. */

	struct Context
	{
		const NAS2D::Font& font;

		NAS2D::Color borderColorNormal = NAS2D::Color{75, 75, 75};
		NAS2D::Color borderColorActive = NAS2D::Color{0, 185, 0};

		NAS2D::Color itemBorderColorMouseHover = NAS2D::Color::DarkGreen;

		NAS2D::Color backgroundColorNormal = NAS2D::Color{0, 85, 0, 220};
		NAS2D::Color backgroundColorSelected = NAS2D::Color{0, 100, 0, 231};

		NAS2D::Color textColorNormal = NAS2D::Color::White;
		NAS2D::Color textColorMouseHover = NAS2D::Color::White;

		unsigned int itemHeight() const;
	};

	void draw(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> itemDrawArea, const Context& context, bool isSelected, bool isHighlighted);
};


/**
 * Implements a ListBox control.
 */
class ListBox : public Control
{
public:
	using SelectionChangedCallback = NAS2D::Signals::Signal<>;
	using ListBoxItem = ListBoxItemText;

	ListBox();
	~ListBox() override;

	bool isEmpty() const;
	std::size_t count() const { return mItems.size(); }

	template <typename... Args>
	void add(Args&&... args) {
		mItems.emplace_back(ListBoxItem{std::forward<Args>(args)...});
		updateScrollLayout();
	}

	void clear();

	bool isItemSelected() const;
	const ListBoxItem& selected() const;
	std::size_t selectedIndex() const { return mSelectedIndex; }
	void setSelected(std::size_t index) { mSelectedIndex = index; mSelectionChanged(); }
	void clearSelected() { mSelectedIndex = constants::NO_SELECTION; }

	template <typename UnaryPredicate>
	void selectIf(UnaryPredicate predicate) {
		for (std::size_t i = 0; i < mItems.size(); ++i) {
			if (predicate(mItems[i])) {
				mSelectedIndex = i;
				return;
			}
		}
	}

	std::size_t currentHighlight() const { return mHighlightIndex; }

	unsigned int lineHeight() const { return mContext.itemHeight(); }

	void update() override;

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

protected:
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	virtual void onMouseMove(int x, int y, int relX, int relY);
	void onMouseWheel(int x, int y);
	virtual void slideChanged(float newPosition);

	void visibilityChanged(bool visible) override;

private:
	void onSizeChanged() override;
	void updateScrollLayout();

	ListBoxItem::Context mContext;

	std::size_t mHighlightIndex = constants::NO_SELECTION;
	std::size_t mSelectedIndex = 0;
	std::size_t mScrollOffsetInPixels = 0;

	std::vector<ListBoxItem> mItems; /**< List of items preserved in the order in which they're added. */

	NAS2D::Rectangle<int> mScrollArea;

	SelectionChangedCallback mSelectionChanged; /**< Callback for selection changed callback. */
	Slider mSlider;
};
