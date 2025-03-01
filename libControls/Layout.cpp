#include "Layout.h"

#include "Control.h"

#include <algorithm>


NAS2D::Vector<int> maxSize(const std::vector<Control*>& controls)
{
	NAS2D::Vector<int> maxSize{0, 0};
	for (const auto* control : controls)
	{
		const auto controlSize = control->size();
		maxSize.x = std::max(maxSize.x, controlSize.x);
		maxSize.y = std::max(maxSize.y, controlSize.y);
	}
	return maxSize;
}


void setSize(const std::vector<Control*>& controls, NAS2D::Vector<int> newSize)
{
	for (auto* control : controls)
	{
		control->size(newSize);
	}
}


void setPositionX(const std::vector<Control*>& controls, int newX)
{
	for (auto* control : controls)
	{
		control->position({newX, control->position().y});
	}
}


void setPositionY(const std::vector<Control*>& controls, int newY)
{
	for (auto* control : controls)
	{
		control->position({control->position().x, newY});
	}
}
