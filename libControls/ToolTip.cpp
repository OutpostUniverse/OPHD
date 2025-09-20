#include "ToolTip.h"

#include <NAS2D/Duration.h>
#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>


namespace
{
	constexpr int MarginTight{2};
	constexpr auto PaddingSize = NAS2D::Vector{MarginTight, MarginTight};


	NAS2D::Rectangle<int> toolTipArea(NAS2D::Rectangle<int> controlArea, NAS2D::Vector<int> textSize, int mouseX)
	{
		const auto toolTipSize = textSize + PaddingSize * 2;

		const auto screenSizeX = NAS2D::Utility<NAS2D::Renderer>::get().size().x;
		const auto maxX = screenSizeX - toolTipSize.x;
		auto toolTipPosition = controlArea.position;
		toolTipPosition.x = (mouseX <= maxX) ? mouseX : maxX;
		if (toolTipPosition.x < 0) { toolTipPosition.x = 0; }

		toolTipPosition.y += (toolTipSize.y <= toolTipPosition.y) ? -toolTipSize.y : controlArea.size.y;

		return {toolTipPosition, toolTipSize};
	}
}


ToolTip::ToolTip():
	mFont{getDefaultFont()},
	mFocus{nullptr}
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
		if (&controlText.control == &control)
		{
			controlText.text = toolTipText;
			return;
		}
	}

	mControlTexts.emplace_back(control, toolTipText);
}


void ToolTip::setToolTipArea(ControlText& controlText, int mouseX)
{
	area(toolTipArea(controlText.control.area(), mFont.size(controlText.text), mouseX));
}


void ToolTip::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative)
{
	if (relative != NAS2D::Vector{0, 0})
	{
		if (mFocus)
		{
			if (mFocus->control.area().contains(position)) { return; }
			else { mFocus = nullptr; }
		}

		mTimer.reset();
	}

	for (auto& controlText : mControlTexts)
	{
		if (mFocus) { break; }
		if (controlText.control.area().contains(position))
		{
			mFocus = &controlText;
			setToolTipArea(controlText, position.x);
			return;
		}
	}

	mFocus = nullptr;
}


void ToolTip::update()
{
	if (mTimer.elapsedTicks() < NAS2D::Duration{1000})
	{
		return;
	}
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	draw(renderer);
}


void ToolTip::draw(NAS2D::Renderer& renderer) const
{
	if (mFocus)
	{
		renderer.drawBoxFilled(area(), NAS2D::Color::DarkGray);
		renderer.drawBox(area(), NAS2D::Color::Black);
		renderer.drawText(mFont, mFocus->text, position() + PaddingSize);
	}
}
