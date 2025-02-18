#include "VerticalLayout.h"


VerticalLayout::VerticalLayout()
{
}


VerticalLayout::VerticalLayout(std::vector<Control*> controls, NAS2D::Vector<int> offset)
{
	for (auto control : controls)
	{
		add(*control, arrangeControl(*control, offset));
	}
}


void VerticalLayout::add(Control& control, NAS2D::Vector<int> offset)
{
	UIContainer::add(control, arrangeControl(control, offset));
}

NAS2D::Vector<int> VerticalLayout::arrangeControl(Control& control, NAS2D::Vector<int> offset)
{
	minimumHorizontalSize = std::max(minimumHorizontalSize, control.size().x);
	nextVerticalPosition += paddingTop;
	NAS2D::Vector<int> newOffset = NAS2D::Vector<int>{0, nextVerticalPosition} + offset;
	nextVerticalPosition += control.size().y + paddingBottom;
	return newOffset;
}
