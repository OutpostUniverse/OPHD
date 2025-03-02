#pragma once

#include "Control.h"

#include <vector>
#include <variant>

class ControlGroup
{
public:
	using ControlList = std::vector<std::variant<Control*, ControlGroup*>>;
	ControlGroup(ControlList);
	void applyVerticalLayout();
	void applyHorizontalLayout();
	NAS2D::Rectangle<int> calculateBoundary();

private:
	ControlList mControlList;
};
