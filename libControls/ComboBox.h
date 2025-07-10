#pragma once

#include "ControlContainer.h"
#include "Button.h"
#include "ListBox.h"
#include "TextField.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Math/Rectangle.h>

#include <cstddef>


namespace NAS2D
{
	enum class MouseButton;
}


class ComboBox : public ControlContainer
{
public:
	using SelectionChangedDelegate = NAS2D::Delegate<void()>;

	ComboBox(SelectionChangedDelegate selectionChangedHandler = {});
	~ComboBox() override;

	void addItem(const std::string& item, int tag = 0);

	std::size_t maxDisplayItems() const { return mMaxDisplayItems; }
	void maxDisplayItems(std::size_t count);

	void clearSelected();

	const std::string& selectionText() const;
	int selectionUserData() const;

	bool isItemSelected() const;
	std::size_t selectedIndex() const;
	void setSelected(std::size_t index);

	void text(const std::string& text);
	const std::string& text() const;

protected:
	void onVisibilityChange(bool visible) override;
	void onResize() override;
	void onMove(NAS2D::Vector<int> displacement) override;
	void onListSelectionChange();

	void onMouseWheel(NAS2D::Vector<int> scrollAmount);
	void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position) override;
	void onDropdownButtonClick();

private:
	Button btnDown;
	ListBox<> lstItems;
	TextField txtField;

	NAS2D::Rectangle<int> mBarRect;

	SelectionChangedDelegate mSelectionChangedHandler;

	std::size_t mMaxDisplayItems;
};
