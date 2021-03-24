#pragma once

#include "TextControl.h"

#include <NAS2D/Renderer/Vector.h>
#include <NAS2D/EventHandler.h>

#include <vector>


/**
 * UI Object that contains other UI Controls.
 * 
 * Generally not intended to be used by itself.
 */
class UIContainer: public Control
{
public:
	UIContainer();
	~UIContainer() override;

	void add(Control& control, NAS2D::Vector<int> offset);
	void clear();

	void bringToFront(Control* control);

	void update() override;

	std::vector<Control*> controls() const;

protected:
	void visibilityChanged(bool visible) override;
	void positionChanged(int dX, int dY) override;

	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

private:
	std::vector<Control*> mControls;
};
