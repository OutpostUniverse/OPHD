// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "UIContainer.h"

#include "RadioButton.h"

#include "../../Common.h"

#include "NAS2D/Utility.h"

#include <algorithm>
#include <iostream>

using namespace NAS2D;

/**
 * C'tor
 */
UIContainer::UIContainer()
{
	Utility<EventHandler>::get().mouseButtonDown().connect(this, &UIContainer::onMouseDown);
}


/**
 * D'tor
 */
UIContainer::~UIContainer()
{
	Utility<EventHandler>::get().mouseButtonDown().disconnect(this, &UIContainer::onMouseDown);
}


/**
 * Adds a Control to the UIContainer.
 */
void UIContainer::add(Control* control, float x, float y)
{
	if(control == nullptr)
	{
		std::cout << "UIContainer::addControl(): Attempting to add a NULL Control." << std::endl;
		return;
	}

	if (std::find(mControls.begin(), mControls.end(), control) != mControls.end())
	{
		std::cout << "UIContainer::addControl(): Duplicate control." << std::endl;
		return;
	}

	if (mControls.size() > 0) { mControls.back()->hasFocus(false); }
	mControls.push_back(control);

	control->position(rect().x() + x, rect().y() + y);
	control->visible(visible());
	control->hasFocus(true);
	if (auto* asRadioButton = dynamic_cast<RadioButton*>(control))
	{
		asRadioButton->parentContainer(this);
	}

	/// todo\	Add validation to contain controls within a UIContainer.
}


/**
 * Drops all controls.
 */
void UIContainer::clear()
{
	mControls.clear();
}


/**
 * 
 */
void UIContainer::bringToFront(Control* control)
{
	auto control_iterator = std::find(mControls.begin(), mControls.end(), control);
	if (control_iterator == mControls.end())
	{
		std::cout << "UIContainer::bringToFront(): Control is not managed by this container." << std::endl; // debug aid, can be pulled in release modes.
		return;
	}

	mControls.back()->hasFocus(false);

	mControls.erase(control_iterator);
	mControls.push_back(control);
	control->hasFocus(true);
}


/**
 * 
 */
void UIContainer::visibilityChanged(bool visible)
{
	for (auto control : mControls) { control->visible(visible); }
}


/**
 * 
 */
void UIContainer::positionChanged(float dX, float dY)
{
	Control::positionChanged(dX, dY);

	for (auto control : mControls)
	{
		control->position(control->position() + NAS2D::Vector{dX, dY});
	}
}


/**
 * 
 */
void UIContainer::onMouseDown(EventHandler::MouseButton /*button*/, int x, int y)
{
	if (!visible()) { return; }

	Control* control = nullptr;
	for (auto it = mControls.rbegin(); it != mControls.rend(); ++it)
	{
		control = (*it);
		if (control->visible() && control->rect().to<int>().contains(NAS2D::Point{x, y}))
		{
			if (control == mControls.back()) { return; }
			bringToFront(control);
			return;
		}
	}
}


/**
 * Updates all Control's in the UIContainer.
 * 
 * \note	This function can be overridden in derived types
 *			but if done, don't forget to update all contained
 *			Control's.
 */
void UIContainer::update()
{
	if (!visible()) { return; }
	for (auto control : mControls) { control->update(); /*if (control->hasFocus()) { Utility<Renderer>::get().drawBox(control->rect(), 255, 0, 255); }*/ }
}

std::vector<Control*> UIContainer::controls() const {
	return mControls;
}
