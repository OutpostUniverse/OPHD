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
	IconGrid(const std::string& filePath, int iconSize, int margin, bool showTooltip = false);
	~IconGrid() override;

	void addItem(const Item&);
	void sort();
	void clear();
	bool isEmpty() const { return mIconItemList.empty(); }

	const std::string& itemName(Index index) const { return mIconItemList[index].name; }
	bool itemExists(const std::string& itemName);
	void removeItem(const std::string& itemName);

	// Getter
	bool itemAvailable(const std::string& itemName);
	// Setter
	void itemAvailable(const std::string& itemName, bool isItemAvailable);

	Index selectedIndex() const { return mSelectedIndex; }

	void clearSelection();
	void setSelection(Index newSelection);
	void setSelectionByMeta(int selectionMetaValue);

	void incrementSelection();
	void decrementSelection();

	Signal::Source& selectionChanged() { return mSelectionChangedSignal; }

	void hide() override;
	void update() override;

protected:
	void onResize() override;

	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	Index translateCoordsToIndex(NAS2D::Vector<int> relativeOffset) const;

	void raiseChangedEvent();

private:
	const NAS2D::RectangleSkin mSkin;
	const NAS2D::Font& mFont;
	const NAS2D::Image& mIconSheet;
	const bool mShowTooltip;

	const int mIconSize;
	const int mIconMargin;
	NAS2D::Vector<int> mGridSizeInIcons;

	std::vector<Item> mIconItemList;

	Index mHighlightIndex = NoSelection;
	Index mSelectedIndex = NoSelection;

	Signal mSelectionChangedSignal;
};
