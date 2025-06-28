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


void ToolTip::add(const Control& control, const std::string& toolTipText)
{
	for (auto& controlText : mControlTexts)
	{
		if (controlText.control == &control)
		{
			controlText.text = toolTipText;
			return;
		}
	}

	mControlTexts.push_back({&control, toolTipText});
}


void ToolTip::buildDrawParams(ControlText& controlText, int mouseX)
{
	const auto toolTipSize = mFont.size(controlText.text) + PaddingSize * 2;

	const auto screenSizeX = NAS2D::Utility<NAS2D::Renderer>::get().size().x;
	const auto maxX = screenSizeX - toolTipSize.x;
	auto toolTipPosition = controlText.control->position();
	toolTipPosition.x = (mouseX <= maxX) ? mouseX : maxX;
	if (toolTipPosition.x < 0) { toolTipPosition.x = 0; }

	toolTipPosition.y += (toolTipSize.y <= toolTipPosition.y) ? -toolTipSize.y : controlText.control->size().y;

	area({toolTipPosition, toolTipSize});
}


void ToolTip::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative)
{
	if (relative != NAS2D::Vector{0, 0})
	{
		if (mFocus)
		{
			if (mFocus->control->area().contains(position)) { return; }
			else { mFocus = nullptr; }
		}

		mTimer.reset();
	}

	for (auto& controlText : mControlTexts)
	{
		if (mFocus) { break; }
		if (controlText.control->area().contains(position))
		{
			mFocus = &controlText;
			buildDrawParams(controlText, position.x);
			return;
		}
	}

	mFocus = nullptr;
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
	if (mFocus)
	{
		auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
		renderer.drawBoxFilled(area(), NAS2D::Color::DarkGray);
		renderer.drawBox(area(), NAS2D::Color::Black);
		renderer.drawText(mFont, mFocus->text, position() + PaddingSize);
	}
}
