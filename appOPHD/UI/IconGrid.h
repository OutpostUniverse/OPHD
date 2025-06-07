#pragma once

#include <libControls/Control.h>

#include "../Constants/UiConstants.h"

#include <NAS2D/Signal/Signal.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Renderer/RectangleSkin.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>

#include <algorithm>


/**
 * Represents a 2D grid of Icon items that can be selected with a mouse.
 */
class IconGrid : public Control
{
public:

	/**
	 * Item used within the IconGrid.
	 */
	struct Item
	{
		std::string name{};
		int sheetId{0};
		int meta{0}; /**< Optional User defined value */
		bool available{true};
		NAS2D::Point<int> pos{};
	};

	using Signal = NAS2D::Signal<const Item*>;
	using Index = std::vector<Item>::size_type;

	static const Index NoSelection;

public:
	IconGrid(const std::string& filePath, int iconSize, int margin);
	~IconGrid() override;

	const std::string& itemName(Index index) const { return mIconItemList[index].name; }

	Index selectedIndex() const { return mSelectedIndex; }

	bool empty() const { return mIconItemList.empty(); }

	void addItem(const Item&);

	void removeItem(const std::string& itemName);
	bool itemExists(const std::string& itemName);
	void clear();

	// Setter
	void itemAvailable(const std::string& itemName, bool isItemAvailable);
	// Getter
	bool itemAvailable(const std::string& itemName);

	void showTooltip(bool value) { mShowTooltip = value; }

	void clearSelection();
	void selection(Index newSelection);
	void selection_meta(int selectionMetaValue);

	void incrementSelection();
	void decrementSelection();

	Signal::Source& selectionChanged() { return mSelectionChangedSignal; }

	void hide() override;

	void sort();

	void update() override;

protected:
	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	void onResize() override;

	void updateGrid();
	Index translateCoordsToIndex(NAS2D::Vector<int> relativeOffset) const;

	void raiseChangedEvent();

private:
	const NAS2D::RectangleSkin mSkin;
	const NAS2D::Font& mFont;
	const NAS2D::Image& mIconSheet;

	int mIconSize = 1;
	int mIconMargin = 0;
	NAS2D::Vector<int> mGridSizeInIcons;

	std::vector<Item> mIconItemList;

	Index mHighlightIndex = NoSelection;
	Index mSelectedIndex = NoSelection;

	bool mShowTooltip = false;

	Signal mSelectionChangedSignal;
};
