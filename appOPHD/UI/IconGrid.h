#pragma once

#include <libControls/Control.h>

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/EnumMouseButton.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/RectangleSkin.h>
#include <NAS2D/Resource/Font.h>
#include <NAS2D/Resource/Image.h>

#include <vector>


struct IconGridItem
{
	std::string name{};
	int sheetId{0};
	int meta{0}; /**< Optional User defined value */
	bool available{true};
	NAS2D::Point<int> pos{};
};


class IconGrid : public Control
{
public:
	using Delegate = NAS2D::Delegate<void(const IconGridItem*)>;
	using Index = std::vector<IconGridItem>::size_type;

	static const Index NoSelection;

public:
	IconGrid(Delegate selectionChangedHandler, const std::string& filePath, int iconSize, int margin, bool showTooltip = false);
	~IconGrid() override;

	void addItem(const IconGridItem&);
	void sort();
	void clear();
	bool isEmpty() const { return mIconItemList.empty(); }

	const std::string& itemName(Index index) const { return mIconItemList[index].name; }
	bool itemExists(const std::string& itemName) const;
	void removeItem(const std::string& itemName);

	// Getter
	bool itemAvailable(const std::string& itemName) const;
	// Setter
	void itemAvailable(const std::string& itemName, bool isItemAvailable);

	Index selectedIndex() const { return mSelectedIndex; }

	void clearSelection();
	void setSelection(Index newSelection);
	void setSelectionByMeta(int selectionMetaValue);

	void incrementSelection();
	void decrementSelection();

	void hide() override;
	void draw() const override;

protected:
	void onResize() override;

	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);
	virtual void onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative);

	bool isInGridArea(NAS2D::Point<int> position) const;
	Index positionToIndex(NAS2D::Point<int> position) const;
	NAS2D::Point<int> indexToPosition(Index index) const;
	NAS2D::Rectangle<int> indexToArea(Index index) const;

	void setSelectionInternal(Index newSelection);

private:
	const NAS2D::RectangleSkin mSkin;
	const NAS2D::Font& mFont;
	const NAS2D::Image& mIconSheet;
	const bool mShowTooltip;

	const int mIconSize;
	const int mIconMargin;
	NAS2D::Vector<int> mGridSizeInIcons;

	std::vector<IconGridItem> mIconItemList;

	Index mHighlightIndex = NoSelection;
	Index mSelectedIndex = NoSelection;

	Delegate mSelectionChangedDelegate;
};
