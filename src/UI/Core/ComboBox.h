#pragma once

#include "ListBox.h"
#include "TextField.h"

class ComboBox : public Control
{
public:
	ComboBox();
	virtual ~ComboBox();

	virtual void update();

	void addItem(const std::string& item);

	int maxDisplayItems() const { return mMaxDisplayItems; }
	void maxDisplayItems(int count);

private:
	void init();

	void resizedHandler(Control*);
	void repositioned(float, float);
	void selectionChanged();

	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y) final;
	virtual void onFontChanged() final;

private:
	Button				btnDown;
	ListBox				lstItems;
	TextField			txtField;

	int					mMaxDisplayItems = constants::MINIMUM_DISPLAY_ITEMS;
};
