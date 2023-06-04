#include "ToolTip.h"

#include "../../Constants/UiConstants.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


namespace
{
	constexpr auto paddingSize = NAS2D::Vector{constants::MarginTight, constants::MarginTight};
}


ToolTip::ToolTip():
	mFont{getDefaultFont()}
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().connect({this, &ToolTip::onMouseMove});
}


ToolTip::~ToolTip()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseMotion().disconnect({this, &ToolTip::onMouseMove});
}


void ToolTip::add(Control& c, const std::string& str)
{
	for (auto& item : mControls)
	{
		if (item.first == &c)
		{
			item.second = str;
			return;
		}
	}

	mControls.push_back(std::make_pair(&c, str));
}


void ToolTip::buildDrawParams(std::pair<Control*, std::string>& item, int mouseX)
{
	const auto toolTipSize = mFont.size(item.second) + paddingSize * 2;

	auto tooltipPosition = item.first->position();
	tooltipPosition.x = mouseX;

	auto offset = NAS2D::Vector{0, -toolTipSize.y};

	if (tooltipPosition.y + offset.y < 0)
	{
		offset.y = toolTipSize.y;
	}


	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto maxX = renderer.size().x - toolTipSize.x;
	if (tooltipPosition.x > maxX)
	{
		offset.x = maxX - tooltipPosition.x;
	}

	position(tooltipPosition + offset);
	size(toolTipSize);
}


void ToolTip::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative)
{
	if (relative != NAS2D::Vector{0, 0})
	{
		if (mFocusedControl)
		{
			if (mFocusedControl->first->rect().contains(position)) { return; }
			else { mFocusedControl = nullptr; }
		}

		mTimer.reset();
	}

	for (auto& item : mControls)
	{
		if (mFocusedControl) { break; }
		if (item.first->rect().contains(position))
		{
			mFocusedControl = &item;
			buildDrawParams(item, position.x);
			return;
		}
	}

	mFocusedControl = nullptr;
}


void ToolTip::update()
{
	if (mTimer.elapsedTicks() < 1000)
	{
		return;
	}
	draw();
}


void ToolTip::draw() const
{
	if (mFocusedControl)
	{
		auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
		renderer.drawBoxFilled(rect(), NAS2D::Color::DarkGray);
		renderer.drawBox(rect(), NAS2D::Color::Black);
		renderer.drawText(mFont, mFocusedControl->second, position() + paddingSize);
	}
}
