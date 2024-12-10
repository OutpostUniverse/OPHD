#include "ToolTip.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


namespace
{
	constexpr int MarginTight{2};
	constexpr auto PaddingSize = NAS2D::Vector{MarginTight, MarginTight};
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
			item.second.push_back(str);
			return;
		}
	}

	auto strVec = std::vector<std::string>{str};

	mControls.push_back(std::make_pair(&c, strVec));
}


void ToolTip::buildDrawParams(std::pair<Control*, std::vector<std::string>>& item, int mouseX)
{
	auto toolTipSize = NAS2D::Vector{0, 0};

	const auto& strVec = item.second;
	for (const auto& str : strVec)
	{
		const auto textSize = mFont.size(str) + PaddingSize * 2;
		toolTipSize.x = std::max(toolTipSize.x, textSize.x);
		toolTipSize.y += textSize.y;
	}

	auto tooltipPosition = item.first->position();

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto maxX = renderer.size().x - toolTipSize.x;
	tooltipPosition.x = (mouseX > maxX) ? maxX : mouseX;

	tooltipPosition.y += (tooltipPosition.y < toolTipSize.y) ? toolTipSize.y : -toolTipSize.y;

	position(tooltipPosition);
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

		auto linePosition = position();
		for (const auto& str : mFocusedControl->second)
		{
			renderer.drawText(mFont, str, linePosition + PaddingSize);
			linePosition.y += mFont.height() + PaddingSize.y * 2;
		}
	}
}
