#pragma once

#include "Control.h"

#include <vector>


namespace NAS2D
{
	enum class MouseButton;

	template <typename BaseType> struct Point;
	template <typename BaseType> struct Vector;
}


/**
 * Control Object that contains other Control objects.
 *
 * Generally not intended to be used by itself.
 */
class ControlContainer : public Control
{
protected:
	ControlContainer();
	ControlContainer(std::vector<Control*> controls);
	~ControlContainer() override;

	void add(Control& control, NAS2D::Vector<int> offset);
	void clear();

	void bringToFront(Control* control);

	void update() override;

protected:
	void onVisibilityChange(bool visible) override;
	void onMove(NAS2D::Vector<int> displacement) override;

	virtual void onMouseDown(NAS2D::MouseButton button, NAS2D::Point<int> position);

private:
	std::vector<Control*> mControls;
};
