#include "RadioButtonGroup.h"

#include "../../Cache.h"
#include "../../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Math/MathUtils.h>

#include <algorithm>


using namespace NAS2D;


RadioButtonGroup::RadioButtonGroup(std::vector<ButtonInfo> buttonInfos)
{
	mRadioButtons.reserve(buttonInfos.size());
	for (auto &buttonInfo : buttonInfos)
	{
		NAS2D::Vector<int> offset = {0, 13};
		offset.y = static_cast<int>(mRadioButtons.size()) * offset.y;

		auto &button = mRadioButtons.emplace_back(this, buttonInfo.name, buttonInfo.delegate);
		button.visible(visible());
		button.position(mRect.startPoint() + offset);
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
	if (mIndex != constants::NoSelection)
	{
		mRadioButtons[mIndex].checked(false);
	}
	mIndex = constants::NoSelection;
}


void RadioButtonGroup::select(std::size_t index)
{
	clear();
	mIndex = index;
	mRadioButtons[index].checked(true);
}


void RadioButtonGroup::select(RadioButtonGroup::RadioButton& button)
{
	auto index = std::distance(mRadioButtons.data(), &button);
	select(index);
}


void RadioButtonGroup::update()
{
	if (!visible()) { return; }

	for (auto &control : mRadioButtons)
	{
		control.update();
	}
}
