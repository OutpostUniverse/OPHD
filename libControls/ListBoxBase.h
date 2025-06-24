#pragma once

#include "Control.h"
#include "ScrollBar.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Color.h>

#include <string>
#include <cstddef>


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
	using SelectionChangedDelegate = NAS2D::Delegate<void()>;

	static const std::size_t NoSelection;


	ListBoxBase(SelectionChangedDelegate selectionChangedHandler = {});
	~ListBoxBase() override;

	bool isEmpty() const;
	virtual std::size_t count() const = 0;

	std::size_t currentHighlight() const;
	std::size_t selectedIndex() const;
	bool isItemSelected() const;
	void setSelection(std::size_t selection);
	void clearSelected();

	void update() override;

protected:
	void draw() const override;
	void drawItems() const;

	void clear();
	void updateScrollLayout();

	void onVisibilityChange(bool visible) override;
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

private:
	std::size_t mHighlightIndex = NoSelection;
	std::size_t mSelectedIndex = NoSelection;
	unsigned int mScrollOffsetInPixels = 0;

	int mItemHeight = 1;
	int mItemWidth = 0;

	SelectionChangedDelegate mSelectionChangedHandler;
	ScrollBar mScrollBar;
};
