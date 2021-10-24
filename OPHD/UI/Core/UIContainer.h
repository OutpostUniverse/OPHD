#pragma once

#include "Control.h"

#include <NAS2D/Math/Vector.h>
#include <NAS2D/EventHandler.h>

#include <vector>


/**
 * UI Object that contains other UI Controls.
 * 
 * Generally not intended to be used by itself.
 */
class UIContainer : public Control
{
protected:
	UIContainer();
	UIContainer(std::vector<Control*> controls);
	~UIContainer() override;

	void add(Control& control, NAS2D::Vector<int> offset);
	void clear();

	void bringToFront(Control* control);

	void update() override;

	const std::vector<Control*>& controls() const;

protected:
	void onVisibilityChange(bool visible) override;
	void onMove(NAS2D::Vector<int> displacement) override;

	virtual void onMouseDown(NAS2D::EventHandler::MouseButton button, int x, int y);

private:
	std::vector<Control*> mControls;
};
