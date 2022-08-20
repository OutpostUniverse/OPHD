#include "ToolTip.h"

#include "../../Cache.h"
#include "../../Constants/UiConstants.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


ToolTip::ToolTip():
	mFont{fontCache.load(constants::FONT_PRIMARY, constants::FontPrimaryNormal)}
{
	Utility<EventHandler>::get().mouseMotion().connect(this, &ToolTip::onMouseMove);
}


ToolTip::~ToolTip()
{
	Utility<EventHandler>::get().mouseMotion().disconnect(this, &ToolTip::onMouseMove);
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
	constexpr int padding = constants::MarginTight * 2;

	const int tooltipWidth = mFont.width(item.second) + padding;
	const int tooltipHeight = mFont.height() + padding;

	auto tooltipPosition = item.first->position();
	tooltipPosition.x = mouseX;

	auto offset = Vector{0, -tooltipHeight - constants::Margin};

	if (tooltipPosition.y + offset.y < 0)
	{
		offset.y = tooltipHeight + constants::Margin;
	}


	auto& renderer = Utility<Renderer>::get();
	if (tooltipPosition.x + tooltipWidth > renderer.size().x)
	{
		offset.x -= (tooltipPosition.x + tooltipWidth) - (renderer.size().x - constants::Margin);
	}

	position(tooltipPosition + offset);
	size({tooltipWidth, tooltipHeight});
}


void ToolTip::onMouseMove(int x, int y, int dX, int dY)
{
	onMouseMove({x, y}, {dX, dY});
}


void ToolTip::onMouseMove(NAS2D::Point<int> position, NAS2D::Vector<int> relative)
{
	if (relative != Vector{0, 0})
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
	if (mTimer.accumulator() < 1000)
	{
		return;
	}
	draw();
}


void ToolTip::draw() const
{
	if (mFocusedControl)
	{
		auto& renderer = Utility<Renderer>::get();
		renderer.drawBoxFilled(rect(), Color::DarkGray);
		renderer.drawBox(rect(), Color::Black);
		renderer.drawText(mFont, mFocusedControl->second, Point{positionX() + constants::MarginTight, positionY() + constants::MarginTight});
	}
}
