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

	static inline constexpr auto NoSelection{std::numeric_limits<std::size_t>::max()};


	ListBoxBase(const NAS2D::Font& font, const NAS2D::Font& fontBold);
	~ListBoxBase() override;

	bool isEmpty() const;
	virtual std::size_t count() const = 0;

	std::size_t currentHighlight() const;
	std::size_t selectedIndex() const;
	bool isItemSelected() const;
	void setSelection(std::size_t selection);
	void clearSelected();

	SelectionChangeSignal::Source& selectionChanged();

	void update() override;
	void draw() const override;

protected:
	void clear();
	void updateScrollLayout();

	void onVisibilityChange(bool) override;
	void onResize() override;
	void onSlideChange(ScrollBar::ValueType newPosition);
	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);
	void onMouseWheel(NAS2D::Vector<int> scrollAmount);

	unsigned int itemWidth() const;
	unsigned int itemHeight() const;
	void itemHeight(int);

	unsigned int drawOffset() const;
	NAS2D::Vector<int> itemDrawSize() const;
	NAS2D::Point<int> itemDrawPosition(std::size_t index) const;
	NAS2D::Rectangle<int> itemDrawArea(std::size_t index) const;

	virtual NAS2D::Color itemBorderColor(std::size_t index) const;

	virtual void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const = 0;


	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;

private:
	std::size_t mHighlightIndex = NoSelection;
	std::size_t mSelectedIndex = NoSelection;
	unsigned int mScrollOffsetInPixels = 0;

	int mItemHeight = 1;
	int mItemWidth = 0;

	SelectionChangeSignal mSelectionChanged;
	ScrollBar mScrollBar;
};
