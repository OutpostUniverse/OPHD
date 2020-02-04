#pragma once

#include "Control.h"

#include "NAS2D/EventHandler.h"

#include <vector>

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

	void add(Control* c, float x, float y);
	void clear();

	void bringToFront(Control* control);

	virtual void update();

protected:
	virtual void visibilityChanged(bool visible);
	virtual void positionChanged(float dX, float dY);

	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

private:
	std::vector<Control*> mControls;
};
