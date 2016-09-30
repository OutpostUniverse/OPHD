#include "UIContainer.h"


/**
 * C'tor
 */
UIContainer::UIContainer():		mDebug(false)
{
}


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
 *			reference, the returned pointer will be NULL.
 *
 * \note	UIContainer only uses the provided pointer as a reference.
 *			It does no memory management and assumes the user is
 *			handling that.
 */
Control* UIContainer::addControl(const std::string& name, Control* c, float x, float y)
{
	// Naturally barf if NULL.
	if(c == nullptr)
	{
		cout << "UIContainer::addControl(): Attempting to add a NULL Control." << endl;
		return nullptr;
	}

	ControlList::iterator it = mControlList.find(toLowercase(name));
	if(it != mControlList.end())
	{
		cout << "UIContainer::addControl(): Attempting to add a duplicate Control '" << name << "'." << endl;
		return nullptr;
	}

	mControlList[toLowercase(name)] = c;
	c->position(rect().x() + x, rect().y() + y);

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
		cout << "UIContainer::deleteControl(): No Control found by name '" << name << "'." << endl;
		return false;
	}
	else
		mControlList.erase(it);

	return true;
}


Control* UIContainer::control(const std::string& name)
{
	ControlList::iterator it = mControlList.find(toLowercase(name));

	if(it == mControlList.end())
		return NULL;

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
	if(!visible())
		return;
	
	if(mDebug)
		Utility<Renderer>::get().drawBox(rect(), 200, 200, 200);

	for(ControlList::iterator it = mControlList.begin(); it != mControlList.end(); ++it)
	{
		it->second->update();
	}
}


/**
 * 
 */
void UIContainer::visibilityChanged(bool visible)
{
	for(auto it = mControlList.begin(); it != mControlList.end(); ++it)
		it->second->visible(visible);
}


void UIContainer::positionChanged(float dX, float dY)
{
	Control::positionChanged(dX, dY);

	for (auto it = mControlList.begin(); it != mControlList.end(); ++it)
		it->second->position(it->second->positionX() + dX, it->second->positionY() + dY);
}


void UIContainer::onFocusChanged()
{
	for (auto it = mControlList.begin(); it != mControlList.end(); ++it)
		it->second->hasFocus(hasFocus());

}
