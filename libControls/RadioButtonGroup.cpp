#include "RadioButtonGroup.h"

#include <limits>


namespace
{
	constexpr auto iconHeight = 13;
}


const std::size_t RadioButtonGroup::NoSelection{std::numeric_limits<std::size_t>::max()};


RadioButtonGroup::RadioButtonGroup(std::vector<std::string> options, SelectDelegate selectHandler, int verticalSpacing) :
	mSelectHandler{selectHandler}
{
	mRadioButtons.reserve(options.size());
	for (auto& option : options)
	{
		NAS2D::Vector<int> offset = {0, iconHeight + verticalSpacing};
		offset.y = static_cast<int>(mRadioButtons.size()) * offset.y;

		auto &button = mRadioButtons.emplace_back(*this, std::move(option));
		button.visible(visible());
		add(button, offset);
	}
}


void RadioButtonGroup::clear()
{
	onClearSelection();
	onSetSelection(NoSelection);
}


void RadioButtonGroup::select(std::size_t index)
{
	onClearSelection();
	mRadioButtons[index].checked(true);
	onSetSelection(index);
}


void RadioButtonGroup::select(RadioButtonGroup::RadioButton& button)
{
	auto index = static_cast<std::size_t>(std::distance(mRadioButtons.data(), &button));
	select(index);
}


void RadioButtonGroup::onClearSelection()
{
	if (mIndex != NoSelection)
	{
		mRadioButtons[mIndex].checked(false);
	}
}


void RadioButtonGroup::onSetSelection(std::size_t index)
{
	if (mIndex != index)
	{
		mIndex = index;
		if (mSelectHandler)
		{
			mSelectHandler(mIndex);
		}
	}
}
