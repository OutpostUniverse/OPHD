#pragma once

#include "ListBox.h"
#include "TextField.h"

class ComboBox : public Control
{
public:
	ComboBox();
	virtual ~ComboBox();

	void addItem(const std::string& item);

	int maxDisplayItems() const { return mMaxDisplayItems; }
	void maxDisplayItems(int count);

	virtual void update();

private:
	void init();

	void resizedHandler(Control*);
	void repositioned(float, float);
	void selectionChanged();

	void onMouseWheel(int x, int y);
	virtual void onMouseDown(EventHandler::MouseButton button, int x, int y) final;
	virtual void onFontChanged() final;
	//virtual void onFocusChanged() final;

private:
	Button				btnDown;
	ListBox				lstItems;
	TextField			txtField;

	Rectangle_2d		mBaseArea;

	int					mMaxDisplayItems = constants::MINIMUM_DISPLAY_ITEMS;
};
