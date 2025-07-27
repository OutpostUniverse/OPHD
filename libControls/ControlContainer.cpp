#include "ControlContainer.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Vector.h>

#include <algorithm>
#include <stdexcept>


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


/**
 * Drops all controls.
 */
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


/**
 * Updates all Controls in the ControlContainer.
 * 
 * \note	This function can be overridden in derived types
 *			but if done, don't forget to update all contained
 *			Controls.
 */
void ControlContainer::update()
{
	if (!visible()) { return; }
	for (auto control : mControls)
	{
		control->update();
		/*
		if (control->hasFocus())
		{
			NAS2D::Utility<NAS2D::Renderer>::get().drawBox(control->area(), {255, 0, 255});
		}
		*/
	}
}
