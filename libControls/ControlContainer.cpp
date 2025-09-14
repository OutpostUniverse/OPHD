#include "ControlContainer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>

#include <algorithm>
#include <stdexcept>


namespace
{
	bool debugDrawBorders = false;
}


void ControlContainer::setDebugDrawBorders(bool drawBorders)
{
	debugDrawBorders = drawBorders;
}


ControlContainer::ControlContainer() : ControlContainer{{}}
{
}


ControlContainer::ControlContainer(std::vector<Control*> controls) :
	mControls{std::move(controls)}
{
}


ControlContainer::~ControlContainer()
{
}


void ControlContainer::add(Control& control, NAS2D::Vector<int> offset)
{
	if (std::find(mControls.begin(), mControls.end(), &control) != mControls.end())
	{
		throw std::runtime_error("ControlContainer::add(): Duplicate control");
	}

	if (mControls.size() > 0) { mControls.back()->hasFocus(false); }
	mControls.push_back(&control);

	control.position(mRect.position + offset);
	control.visible(visible());
	control.hasFocus(true);
}


void ControlContainer::clear()
{
	mControls.clear();
}


void ControlContainer::onVisibilityChange(bool visible)
{
	for (auto control : mControls) { control->visible(visible); }
}


void ControlContainer::onMove(NAS2D::Vector<int> displacement)
{
	for (auto control : mControls)
	{
		control->position(control->position() + displacement);
	}
}


void ControlContainer::update()
{
	if (!visible()) { return; }
	for (auto control : mControls)
	{
		control->update();
	}

	if (debugDrawBorders)
	{
		drawControlBorders();
	}
}


void ControlContainer::drawControlBorders() const
{
	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();
	for (auto control : mControls)
	{
		const auto borderColor = control->hasFocus() ? NAS2D::Color{255, 0, 255} : NAS2D::Color{255, 255, 255};
		renderer.drawBox(control->area(), borderColor);
	}
}
