#pragma once

#include <libControls/ListBoxBase.h>

#include <NAS2D/Math/Point.h>

#include <string>
#include <vector>


class Factory;


/**
 * Implements a ListBox control.
 */
class FactoryListBox : public ListBoxBase
{
public:
	struct FactoryListBoxItem
	{
		std::string text;
		Factory* factory = nullptr;
		NAS2D::Point<int> icon_slice;
	};


	FactoryListBox(SelectionChangedDelegate selectionChangedHandler = {});

	void addItem(Factory* factory);
	void setSelected(Factory* factory);

	Factory* selectedFactory();

	void clear();

protected:
	void add(std::string textDescription, Factory* newFactory, NAS2D::Point<int> iconPosition);

	virtual std::size_t count() const override;

	const FactoryListBoxItem& getItem(std::size_t index) const;

	virtual NAS2D::Color itemBorderColor(std::size_t index) const override;

	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override;

private:
	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;
	const NAS2D::Image& mStructureIcons;

	std::vector<FactoryListBoxItem> mItems;
};
