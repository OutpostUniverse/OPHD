#pragma once

#include "Control.h"
#include "ScrollBar.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Color.h>

#include <string>
#include <vector>
#include <cstddef>
#include <limits>


/**
 * Implements a base ListBox control that handles the low-level drawing,
 * input handling and management code while leaving the specific implementation
 * to more derived types.
 *
 * \note	This is an abstract class -- it will need to be inherited from
 *			in order to be used.
 */
class ListBoxBase : public Control
{
public:
	using SelectionChangeSignal = NAS2D::Signal<>;

	/**
	 * Derived SpecialListBox types can inherit from this struct
	 * for specialized information needed for derived types.
	 */
	struct ListBoxItem
	{
		ListBoxItem() = default;
		ListBoxItem(std::string initialText) : text(initialText) {}
		virtual ~ListBoxItem() = default;

		std::string text;
	};

	static inline constexpr auto NoSelection{std::numeric_limits<std::size_t>::max()};


	ListBoxBase(const NAS2D::Font& font, const NAS2D::Font& fontBold);
	~ListBoxBase() override;

	bool isEmpty() const;
	std::size_t count() const;

	void clear();

	std::size_t currentHighlight() const;
	std::size_t selectedIndex() const;
	bool isItemSelected() const;
	const ListBoxItem& selected() const;
	void setSelection(std::size_t selection);
	void clearSelected();

	SelectionChangeSignal::Source& selectionChanged();

	void update() override = 0;
	void draw() const override;

protected:
	template <typename ItemType, typename... Args>
	void add(Args&&... args) {
		mItems.emplace_back(new ItemType{std::forward<Args>(args)...});
		updateScrollLayout();
	}

	void updateScrollLayout();

	void onVisibilityChange(bool) override;
	void onResize() override;
	void onSlideChange(ScrollBar::ValueType newPosition);
	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);
	void onMouseWheel(NAS2D::Vector<int> scrollAmount);

	unsigned int itemWidth() const { return static_cast<unsigned int>(mItemWidth); }
	unsigned int itemHeight() const { return static_cast<unsigned int>(mItemHeight); }
	void itemHeight(int);

	unsigned int drawOffset() const { return mScrollOffsetInPixels; }
	NAS2D::Vector<int> itemDrawSize() const;
	NAS2D::Point<int> itemDrawPosition(std::size_t index) const;
	NAS2D::Rectangle<int> itemDrawArea(std::size_t index) const;


	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;

	std::vector<ListBoxItem*> mItems;

private:
	std::size_t mHighlightIndex = NoSelection;
	std::size_t mSelectedIndex = NoSelection;
	unsigned int mScrollOffsetInPixels = 0;

	int mItemHeight = 1;
	int mItemWidth = 0;

	SelectionChangeSignal mSelectionChanged;
	ScrollBar mScrollBar;
};
