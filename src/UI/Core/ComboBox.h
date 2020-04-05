#pragma once

#include "ListBox.h"
#include "TextField.h"

#include "NAS2D/Signal.h"
#include "NAS2D/EventHandler.h"
#include "NAS2D/Renderer/Rectangle.h"

#include <cstddef>


class ComboBox : public Control
{
public:
	using SelectionChanged = NAS2D::Signals::Signal<>;

public:
	ComboBox();
	~ComboBox() override;

	void addItem(const std::string& item, int tag = 0);

	std::size_t maxDisplayItems() const { return mMaxDisplayItems; }
	void maxDisplayItems(std::size_t count);

	void clearSelection();

	SelectionChanged& selectionChanged() { return mSelectionChanged; }

	const std::string& selectionText() const;
	int selectionTag() const;

	std::size_t currentSelection() { return lstItems.currentSelection(); }
	void currentSelection(std::size_t index);

	void update() override;

	void text(const std::string& text);
	const std::string& text() const;

private:
	void init();

	void resizedHandler(Control* control);
	void repositioned(float, float);
	void lstItemsSelectionChanged();

	void onMouseWheel(int x, int y);
	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y) final;

private:
	Button					btnDown;
	ListBox					lstItems;
	TextField				txtField;

	NAS2D::Rectangle_2df    mBaseArea;

	SelectionChanged        mSelectionChanged;

	std::size_t            mMaxDisplayItems = constants::MINIMUM_DISPLAY_ITEMS;
};
