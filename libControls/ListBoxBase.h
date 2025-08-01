#pragma once

#include "Control.h"
#include "ScrollBar.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/Color.h>

#include <string>
#include <cstddef>


namespace NAS2D
{
	enum class MouseButton;
	class Renderer;
}


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


	ListBoxBase(NAS2D::Vector<int> itemSize, SelectionChangedDelegate selectionChangedHandler = {});
	~ListBoxBase() override;

	virtual std::size_t count() const = 0;
	bool isEmpty() const;
	bool isItemSelected() const;
	std::size_t highlightIndex() const;
	std::size_t selectedIndex() const;
	void selectedIndex(std::size_t index);
	void clearSelected();

	void update() override;

protected:
	void draw(NAS2D::Renderer& renderer) const override;
	void drawScrollArea(NAS2D::Renderer& renderer) const;
	virtual void drawItemCell(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const;
	virtual void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const = 0;

	void clear();
	void setSelectedInternal(std::size_t index);
	void updateScrollLayout();

	void onVisibilityChange(bool visible) override;
	void onMove(NAS2D::Vector<int> displacement) override;
	void onResize() override;
	void onSlideChange(int newPosition);
	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);
	void onMouseWheel(NAS2D::Vector<int> scrollAmount);

	virtual NAS2D::Color borderColor() const;
	virtual NAS2D::Color itemBorderColor(std::size_t index) const;
	virtual NAS2D::Color emptyAreaColor() const;

protected:
	ScrollBar mScrollBar;
	NAS2D::Rectangle<int> mScrollArea;
	NAS2D::Vector<int> mItemSize;

	int mScrollOffsetInPixels = 0;
	std::size_t mHighlightIndex = NoSelection;
	std::size_t mSelectedIndex = NoSelection;

	SelectionChangedDelegate mSelectionChangedHandler;
};
