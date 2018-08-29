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
 * 
 * \return	A pointer to the Control that was just added.
 * 
 * \warning	If adding failed (usually due to a duplicate Control
 *			reference, the returned pointer will be nullptr.
 *
 * \note	UIContainer only uses the provided pointer as a reference.
 *			It does no memory management and assumes the user is
 *			handling that.
 */
Control* UIContainer::addControl(const std::string& name, Control* c, float x, float y)
{
	if(c == nullptr)
	{
		std::cout << "UIContainer::addControl(): Attempting to add a NULL Control." << std::endl;
		return nullptr;
	}

	if(mControlList.find(toLowercase(name)) != mControlList.end())
	{
		std::cout << "UIContainer::addControl(): Attempting to add a duplicate Control '" << name << "'." << std::endl;
		return nullptr;
	}

	mControlList[toLowercase(name)] = c;
	mDrawOrder.push_back(c);

	c->position(rect().x() + x, rect().y() + y);
	c->visible(visible());

	/// todo\	Add validation to contain controls within a UIContainer.

	return c;
}


/**
 * Drops all controls.
 */
void UIContainer::dropAllControls()
{
	mControlList.clear();
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
	for (auto control : mDrawOrder) { control->update(); }
}


/**
 * 
 */
void UIContainer::visibilityChanged(bool visible)
{
	for (auto control : mControlList) { control.second->visible(visible); }
}


void UIContainer::positionChanged(float dX, float dY)
{
	Control::positionChanged(dX, dY);

	for (auto control : mControlList)
	{
		control.second->position(control.second->positionX() + dX, control.second->positionY() + dY);
	}
}


void UIContainer::onFocusChanged()
{
	for (auto control : mControlList) { control.second->hasFocus(hasFocus()); }
}
