#pragma once

#include <libControls/ListBoxBase.h>

#include <vector>


enum class StructureState;
class Structure;


/**
 * Implements a ListBox control.
 */
class StructureListBox : public ListBoxBase
{
public:
	struct StructureListBoxItem
	{
		std::string text;
		Structure* structure = nullptr;
		std::string stateDescription;
	};


	StructureListBox(SelectionChangedDelegate selectionChangedHandler = {});

	void addItem(Structure* structure, std::string stateDescription = std::string{});
	void setSelected(const Structure* structure);

	const Structure* selectedStructure() const;
	Structure* selectedStructure();

	StructureListBoxItem* last();

	void clear();

protected:
	void add(Structure* structure, std::string stateDescription);

	virtual std::size_t count() const override;

	const StructureListBoxItem& getItem(std::size_t index) const;

	virtual NAS2D::Color itemBorderColor(std::size_t index) const override;

	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;

	std::vector<StructureListBoxItem> mItems;
};
