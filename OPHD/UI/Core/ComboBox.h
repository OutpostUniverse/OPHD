#pragma once

#include "Button.h"
#include "ListBox.h"
#include "TextField.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Rectangle.h>

#include <cstddef>


class ComboBox : public Control
{
public:
	using SelectionChangeSignal = NAS2D::Signal<>;

	ComboBox();
	~ComboBox() override;

	void addItem(const std::string& item, int tag = 0);

	std::size_t maxDisplayItems() const { return mMaxDisplayItems; }
	void maxDisplayItems(std::size_t count);

	void clearSelected();

	SelectionChangeSignal::Source& selectionChanged() { return mSelectionChanged; }

	const std::string& selectionText() const;
	int selectionTag() const;

	bool isItemSelected() const;
	std::size_t selectedIndex() { return lstItems.selectedIndex(); }
	void setSelected(std::size_t index);

	void update() override;

	void text(const std::string& text);
	const std::string& text() const;

private:
	void onResize() override;
	void onMove(NAS2D::Vector<int> displacement) override;
	void onListSelectionChange();

	void onMouseWheel(int x, int y);
	void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

	Button btnDown;
	ListBox<> lstItems;
	TextField txtField;

	NAS2D::Rectangle<int> mBaseArea;

	SelectionChangeSignal mSelectionChanged;

	std::size_t mMaxDisplayItems = constants::MinimumDisplayItems;
};
