#pragma once

#include <libControls/ListBoxBase.h>

#include <NAS2D/Signal/Signal.h>
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
	struct FactoryListBoxItem : public ListBoxItem
	{
		FactoryListBoxItem(std::string textDescription, Factory* newFactory, NAS2D::Point<int> iconPosition) :
			ListBoxItem{textDescription},
			factory{newFactory},
			icon_slice{iconPosition}
		{}

		Factory* factory = nullptr;
		NAS2D::Point<int> icon_slice;
	};


	FactoryListBox();

	void addItem(Factory* factory);
	void setSelected(Factory*);

	Factory* selectedFactory();

protected:
	FactoryListBoxItem& getItem(std::size_t index) const;

	virtual NAS2D::Color itemBorderColor(std::size_t index) const override;

	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override;

private:
	const NAS2D::Image& mStructureIcons;
};
