#pragma once

#include "Control.h"


/**
 * UI Object that contains other UI Control's.
 * 
 * Generally not intended to be used by itself.
 */
class UIContainer: public Control
{
public:
	UIContainer();
	virtual ~UIContainer();

	Control* addControl(const std::string& name, Control* c, float x, float y);
	bool deleteControl(const std::string& name);

	Control* control(const std::string& name);

	virtual void update();

protected:
	virtual void visibilityChanged(bool visible);
	virtual void positionChanged(float dX, float dY);

	virtual void onFocusChanged();

private:
	typedef std::map<std::string, Control*> ControlList;

	ControlList				mControlList;	/**<  */
};
