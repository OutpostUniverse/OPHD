#include "RadioButtonGroup.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <limits>


const std::size_t RadioButtonGroup::NoSelection{std::numeric_limits<std::size_t>::max()};


RadioButtonGroup::RadioButtonGroup(std::vector<std::string> options, SelectDelegate selectHandler) :
	mSelectHandler{selectHandler}
{
	mRadioButtons.reserve(options.size());
	for (auto& option : options)
	{
		NAS2D::Vector<int> offset = {0, 13};
		offset.y = static_cast<int>(mRadioButtons.size()) * offset.y;

		auto &button = mRadioButtons.emplace_back(*this, std::move(option));
		button.visible(visible());
		button.position(mRect.position + offset);
	}
}


void RadioButtonGroup::onMove(NAS2D::Vector<int> displacement)
{
	Control::onMove(displacement);

	for (auto &control : mRadioButtons)
	{
		control.position(control.position() + displacement);
	}
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


void RadioButtonGroup::update()
{
	if (!visible()) { return; }
	draw();
}


void RadioButtonGroup::draw() const
{
	for (auto &control : mRadioButtons)
	{
		control.draw();
	}
}
