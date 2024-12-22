#include "ToolTip.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <string>
#include <ranges>
#include <iostream>
#include <functional>

namespace
{
	constexpr int MarginTight{2};
	constexpr auto PaddingSize = NAS2D::Vector{MarginTight, MarginTight};

	void processLines(const std::string& text, const std::function<void(const std::string&)>& lineProcessor)
	{
		for (const auto& line : text | std::views::split('\n'))
		{
			std::string lineStr;
			std::ranges::copy(line, std::back_inserter(lineStr));
			lineProcessor(lineStr);
		}
	}
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
	const auto toolTipLineHeight = mFont.height();
	const auto numberOfLines = static_cast<int>(std::count(item.second.begin(), item.second.end(), '\n') + 1);
	const auto toolTipTextHeight = toolTipLineHeight * numberOfLines;

	auto calculateMaxWidth = [this](const std::string& text) {
		int maxWidth = 0;
		processLines(text, [&maxWidth, this](const std::string& line)
		{
			maxWidth = std::max(maxWidth, mFont.size(line).x);
		});
		return maxWidth;
	};

	const auto toolTipTextWidth = calculateMaxWidth(item.second);

	const auto toolTipSize = NAS2D::Vector{toolTipTextWidth, toolTipTextHeight} + PaddingSize * 2;

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

		auto linePosition = position() + PaddingSize;

		processLines(mFocusedControl->second, [&renderer, &linePosition, this](const std::string& line)
		{
			renderer.drawText(mFont, line, linePosition);
			linePosition.y += mFont.height();
		});
	}
}
