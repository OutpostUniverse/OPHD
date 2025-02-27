#pragma once

#include <libControls/ListBoxBase.h>

#include <NAS2D/Signal/Signal.h>

#include <vector>


enum class StructureState;
class Structure;


/**
 * Implements a ListBox control.
 */
class StructureListBox : public ListBoxBase
{
public:
	using SelectionChangedSignal = NAS2D::Signal<Structure*>;

	struct StructureListBoxItem
	{
		std::string text;
		Structure* structure = nullptr;
		std::string stateDescription;
	};


	StructureListBox();

	void addItem(Structure* structure, std::string stateDescription = std::string{});
	void setSelected(const Structure*);

	Structure* selectedStructure();

	StructureListBoxItem* last();

	void clear();

protected:
	void add(Structure* s, std::string stateDescription);

	virtual std::size_t count() const override;

	const StructureListBoxItem& getItem(std::size_t index) const;

	virtual NAS2D::Color itemBorderColor(std::size_t index) const override;

	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override;

private:
	std::vector<StructureListBoxItem> mItems;
};
