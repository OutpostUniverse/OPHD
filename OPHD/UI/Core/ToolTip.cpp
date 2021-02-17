#include "ToolTip.h"

#include "../../Cache.h"
#include "../../Constants.h"

#include <NAS2D/EventHandler.h>
#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


ToolTip::ToolTip():
	mFont{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL) }
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


void ToolTip::onMouseMove(int x, int y, int dX, int dY)
{
	if (dX != 0 || dY != 0)
	{
		mTimer.reset();
	}

	mMouseCoords = { x, y };

	for (auto& item : mControls)
	{
		if (item.first->rect().contains(mMouseCoords))
		{
			mFocusedControl = &item;
			return;
		}
	}

	mFocusedControl = nullptr;
}


void ToolTip::update()
{
	if (mTimer.accumulator() < 2000)
	{
		return;
	}

	if (mFocusedControl)
	{
		auto& renderer = Utility<Renderer>::get();

		const Rectangle r = { 0, 0, 100, 100 };

		renderer.drawBoxFilled(r, Color::Navy);
		renderer.drawText(mFont, mFocusedControl->second, Point{ r.x + constants::MARGIN, r.y + constants::MARGIN });
	}
}
