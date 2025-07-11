#include "ControlContainer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/EventHandler.h>
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
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().connect({this, &ControlContainer::onMouseDown});
}


ControlContainer::~ControlContainer()
{
	NAS2D::Utility<NAS2D::EventHandler>::get().mouseButtonDown().disconnect({this, &ControlContainer::onMouseDown});
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


void ControlContainer::bringToFront(Control* control)
{
	auto control_iterator = std::find(mControls.begin(), mControls.end(), control);
	if (control_iterator == mControls.end())
	{
		throw std::runtime_error("ControlContainer::bringToFront(): Control is not managed by this container.");
	}

	mControls.back()->hasFocus(false);

	mControls.erase(control_iterator);
	mControls.push_back(control);
	control->hasFocus(true);
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


void ControlContainer::onMouseDown(NAS2D::MouseButton /*button*/, NAS2D::Point<int> position)
{
	if (!visible()) { return; }

	Control* control = nullptr;
	for (auto it = mControls.rbegin(); it != mControls.rend(); ++it)
	{
		control = (*it);
		if (control->visible() && control->area().contains(position))
		{
			if (control == mControls.back()) { return; }
			bringToFront(control);
			return;
		}
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


const std::vector<Control*>& ControlContainer::controls() const {
	return mControls;
}
