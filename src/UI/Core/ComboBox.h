#pragma once

#include "ListBox.h"
#include "TextField.h"

class ComboBox : public Control
{
public:
	using SelectionChanged = NAS2D::Signals::Signal0<void>;

public:
	ComboBox();
	virtual ~ComboBox();

	void addItem(const std::string& item, int tag = 0);

	int maxDisplayItems() const { return mMaxDisplayItems; }
	void maxDisplayItems(int count);

	void clearSelection();

	SelectionChanged& selectionChanged() { return mSelectionChanged; }

	const std::string& selectionText() const;
	int selectionTag() const;

	int currentSelection() { return lstItems.currentSelection(); }

	virtual void update();

private:
	void init();

	void resizedHandler(Control*);
	void repositioned(float, float);
	void lstItemsSelectionChanged();

	void onMouseWheel(int x, int y);
	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y) final;
	virtual void onFontChanged() final;

private:
	Button				btnDown;
	ListBox				lstItems;
	TextField			txtField;

	Rectangle_2d		mBaseArea;

	SelectionChanged	mSelectionChanged;

	int					mMaxDisplayItems = constants::MINIMUM_DISPLAY_ITEMS;
};
