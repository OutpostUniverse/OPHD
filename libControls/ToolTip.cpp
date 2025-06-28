#include "ToolTip.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <string>

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


void ToolTip::add(const Control& c, const std::string& str)
{
	for (auto& item : mControls)
	{
		if (item.control == &c)
		{
			item.text = str;
			return;
		}
	}

	mControls.push_back({&c, str});
}


void ToolTip::buildDrawParams(ControlText& item, int mouseX)
{
	const auto toolTipSize = mFont.size(item.text) + PaddingSize * 2;

	auto toolTipPosition = item.control->position();

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	const auto maxX = renderer.size().x - toolTipSize.x;
	toolTipPosition.x = (mouseX <= maxX) ? mouseX : maxX;
	if (toolTipPosition.x < 0) { toolTipPosition.x = 0; }

	toolTipPosition.y += (toolTipSize.y <= toolTipPosition.y) ? -toolTipSize.y : item.control->size().y;

	area({toolTipPosition, toolTipSize});
}


void ToolTip::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative)
{
	if (relative != NAS2D::Vector{0, 0})
	{
		if (mFocusedControl)
		{
			if (mFocusedControl->control->area().contains(position)) { return; }
			else { mFocusedControl = nullptr; }
		}

		mTimer.reset();
	}

	for (auto& item : mControls)
	{
		if (mFocusedControl) { break; }
		if (item.control->area().contains(position))
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
		renderer.drawBoxFilled(area(), NAS2D::Color::DarkGray);
		renderer.drawBox(area(), NAS2D::Color::Black);
		renderer.drawText(mFont, mFocusedControl->text, position() + PaddingSize);
	}
}
