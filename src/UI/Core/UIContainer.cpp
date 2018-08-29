// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "UIContainer.h"

#include <iostream>

/**
 * C'tor
 */
UIContainer::UIContainer()
{}


/**
 * D'tor
 */
UIContainer::~UIContainer()
{}


/**
 * Adds a Control to the UIContainer.
 */
void UIContainer::add(Control* c, float x, float y)
{
	if(c == nullptr)
	{
		std::cout << "UIContainer::addControl(): Attempting to add a NULL Control." << std::endl;
		return;
	}

	if (find(mControls.begin(), mControls.end(), c) != mControls.end())
	{
		std::cout << "UIContainer::addControl(): Duplicate control." << std::endl;
		return;
	}

	mControls.push_back(c);

	c->position(rect().x() + x, rect().y() + y);
	c->visible(visible());

	/// todo\	Add validation to contain controls within a UIContainer.
}


/**
 * Drops all controls.
 */
void UIContainer::clear()
{
	mControls.clear();
}


void UIContainer::bringToFront(Control* _c)
{
	auto control_iterator = find(mControls.begin(), mControls.end(), _c);
	if (control_iterator == mControls.end())
	{
		std::cout << "UIContainer::bringToFront(): Control is not managed by this container." << std::endl; // debug aid, can be pulled in release modes.
		return;
	}

	mControls.back()->hasFocus(false);

	mControls.erase(control_iterator);
	mControls.push_back(_c);
	_c->hasFocus(true);
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
	for (auto control : mControls) { control->update(); }
}


/**
 * 
 */
void UIContainer::visibilityChanged(bool visible)
{
	for (auto control : mControls) { control->visible(visible); }
}


void UIContainer::positionChanged(float dX, float dY)
{
	Control::positionChanged(dX, dY);

	for (auto control : mControls)
	{
		control->position(control->positionX() + dX, control->positionY() + dY);
	}
}
