#pragma once

#include "Control.h"

#include <vector>


namespace NAS2D
{
	enum class MouseButton;

	template <typename BaseType> struct Point;
	template <typename BaseType> struct Vector;
}


class ControlContainer : public Control
{
public:
	static void setDebugDrawBorders(bool draw);

protected:
	ControlContainer();
	ControlContainer(std::vector<Control*> controls);
	~ControlContainer() override;

	void add(Control& control, NAS2D::Vector<int> offset);
	void clear();

	void update() override;

protected:
	void onVisibilityChange(bool visible) override;
	void onMove(NAS2D::Vector<int> displacement) override;

	void drawControlBorders() const;

private:
	std::vector<Control*> mControls;
};
