#include "RadioButtonGroup.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>
#include <limits>


const std::size_t RadioButtonGroup::NoSelection{std::numeric_limits<std::size_t>::max()};


RadioButtonGroup::RadioButtonGroup(std::vector<ButtonInfo> buttonInfos)
{
	mRadioButtons.reserve(buttonInfos.size());
	for (auto &buttonInfo : buttonInfos)
	{
		NAS2D::Vector<int> offset = {0, 13};
		offset.y = static_cast<int>(mRadioButtons.size()) * offset.y;

		auto &button = mRadioButtons.emplace_back(*this, buttonInfo.name, buttonInfo.delegate);
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


void RadioButtonGroup::clear()
{
	if (mIndex != NoSelection)
	{
		mRadioButtons[mIndex].checked(false);
	}
	mIndex = NoSelection;
}


void RadioButtonGroup::select(std::size_t index)
{
	clear();
	mIndex = index;
	mRadioButtons[index].checked(true);
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
