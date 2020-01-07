#pragma once

#include "NAS2D/NAS2D.h"

#include "UIContainer.h"
#include "Slider.h"

#include "../../Constants/UiConstants.h"

#include <string>
#include <vector>


/**
 * Implements a base ListBox control that handles the low-level drawing,
 * input handling and management code while leaving the specific implementation
 * to more derived types.
 * 
 * \notes	This is a pure virtual function -- it will need to be inherited from
 *			in order to be used.
 */
class ListBoxBase : public UIContainer
{
public:
	/**
	 * Callback signal fired whenever the list selection changes.
	 */
	using SelectionChangedCallback = NAS2D::Signals::Signal0<void>;

public:
	/**
	 * Derived SpecialListBox types can inherit from this struct
	 * for specialized information needed for derived types.
	 */
	class ListBoxItem
	{
	public:
		ListBoxItem() = default;
		ListBoxItem(std::string text) : Text(text) {}
		virtual ~ListBoxItem() = default;

	public:
		std::string Text;
		NAS2D::Point_2d	Size;
	};

public:
	ListBoxBase();
	virtual ~ListBoxBase();

	void addItem(ListBoxItem*);
	void removeItem(ListBoxItem*);
	void clearItems();

	size_t count() const;
	bool empty() const;

	uint32_t currentHighlight() const;
	uint32_t currentSelection() const;
	void setSelection(int selection);

	const std::string& selectionText() const;

	void clearSelection();

	SelectionChangedCallback& selectionChanged() { return mSelectionChanged; }

	virtual void update() = 0;

protected:
	/**
	 * List of ListBoxItem's.
	 * 
	 * \note	Pointers used for polymorphism.
	 * 
	 * \note	SpecialListBox owns pointers in this list and will
	 *			handle memory internally.
	 */
	using ItemList = std::vector<ListBoxItem*>;

protected:
	void _update_item_display();

	uint32_t item_width() const { return mItemWidth; }
	uint32_t item_height() const { return mItemHeight; }
	void item_height(int);

	uint32_t draw_offset() const { return mCurrentOffset; }

	virtual void visibilityChanged(bool) final;


protected:
	ItemList					mItems;											/**< List of Items. */

private:
	void _init();
	void slideChanged(double _position);

	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);
	void onMouseMove(int x, int y, int relX, int relY);
	void onMouseWheel(int x, int y);

	virtual void onSizeChanged() final;

private:
	uint32_t					mCurrentHighlight = constants::NO_SELECTION;	/**< Currently highlighted selection index. */
	uint32_t					mCurrentSelection = constants::NO_SELECTION;	/**< Current selection index. */
	uint32_t					mCurrentOffset = 0;								/**< Draw Offset. */

	uint32_t					mItemHeight = 1;								/**< Height of a ListBoxItem. */
	uint32_t					mItemWidth = 0;									/**< Width of a ListBoxItem. */
	uint32_t					mLineCount = 0;									/**< Number of lines that can be displayed. */

	NAS2D::Point_2d				mMousePosition;									/**< Mouse coordinates. */

	NAS2D::Color				mText = NAS2D::Color::White;					/**< Text Color */
	NAS2D::Color				mHighlightBg = NAS2D::Color::Green;				/**< Highlight Background color. */
	NAS2D::Color				mHighlightText = NAS2D::Color::White;			/**< Text Color for an item that is currently highlighted. */

	SelectionChangedCallback	mSelectionChanged;								/**< Callback for selection changed callback. */
	Slider						mSlider;										/**< Slider control. */
};