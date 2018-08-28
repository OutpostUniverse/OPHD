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
	c->position(rect().x() + x, rect().y() + y);


	mDrawOrder.push_back(c);

	/// todo\	Add validation to contain controls within a UIContainer.

	return c;
}


/**
 * Deletes a given Control from the UIContainer.
 * 
 * \param	c	Pointer to a Control to delete.
 * 
 * \return	Returns true of Control is deleted. Otherwise returns false.
 */
bool UIContainer::deleteControl(const std::string& name)
{
	ControlList::iterator it = mControlList.find(toLowercase(name));
	if(it == mControlList.end())
	{
		std::cout << "UIContainer::deleteControl(): No Control found by name '" << name << "'." << std::endl;
		return false;
	}
	else
	{
		mDrawOrder.erase(find(mDrawOrder.begin(), mDrawOrder.end(), it->second));
		mControlList.erase(it);
	}

	return true;
}


/**
 * Drops all controls.
 */
void UIContainer::dropAllControls()
{
	mControlList.clear();
}


/**
 * Gets a pointer to a Control by name.
 * 
 * \return	Returns a pointer to a Control or nullptr if the named control wasn't found.
 */
Control* UIContainer::control(const std::string& name)
{
	ControlList::iterator it = mControlList.find(toLowercase(name));

	if (it == mControlList.end()) { return nullptr; }

	return it->second;
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
