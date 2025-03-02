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
	void applyOffset(NAS2D::Vector<int> offset);
	NAS2D::Rectangle<int> calculateBoundary();

private:
	ControlList mControlList;
};
