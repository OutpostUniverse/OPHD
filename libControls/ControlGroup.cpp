#include "ControlGroup.h"

ControlGroup::ControlGroup(ControlList controls) :
	mControlList(controls)
{}


void ControlGroup::applyVerticalLayout()
{
	NAS2D::Vector<int> yPositionOffset = {0, 0};
	for (auto control : mControlList)
	{
		if(std::holds_alternative<Control*>(control))
		{
			std::get<Control*>(control)->position(std::get<Control*>(control)->position() + yPositionOffset);
			yPositionOffset = yPositionOffset + NAS2D::Vector<int>{0, std::get<Control*>(control)->size().y};
		}
		else
		{
			std::get<ControlGroup*>(control)->applyOffset(yPositionOffset);
			yPositionOffset = yPositionOffset + NAS2D::Vector<int>{0, std::get<ControlGroup*>(control)->calculateBoundary().size.y};
		}
	}
}


void ControlGroup::applyHorizontalLayout()
{
	NAS2D::Vector<int> xPositionOffset = {0, 0};
	for (auto control : mControlList)
	{
		if(std::holds_alternative<Control*>(control))
		{
			std::get<Control*>(control)->position(std::get<Control*>(control)->position() + xPositionOffset);
			xPositionOffset = xPositionOffset + NAS2D::Vector<int>{std::get<Control*>(control)->size().x, 0};
		}
		else
		{
			std::get<ControlGroup*>(control)->applyOffset(xPositionOffset);
			xPositionOffset = xPositionOffset + NAS2D::Vector<int>{std::get<ControlGroup*>(control)->calculateBoundary().size.x, 0};
		}
	}
}


NAS2D::Rectangle<int> ControlGroup::calculateBoundary()
{
	NAS2D::Rectangle<int> boundary = {0, 0, 0, 0};
	if(mControlList.empty()) { return boundary; }

	if(std::holds_alternative<Control*>(mControlList.front()))
	{
		boundary = std::get<Control*>(mControlList.front())->area();
	}
	else
	{
		boundary = std::get<ControlGroup*>(mControlList.front())->calculateBoundary();
	}

	for (auto control : mControlList)
	{
		if(std::holds_alternative<Control*>(control))
		{
			boundary.position.x = std::min(boundary.position.x, std::get<Control*>(control)->position().x);
			boundary.position.y = std::min(boundary.position.y, std::get<Control*>(control)->position().y);
			boundary.size.x = std::max(boundary.size.x, std::get<Control*>(control)->position().x + std::get<Control*>(control)->size().x);
			boundary.size.y = std::max(boundary.size.y, std::get<Control*>(control)->size().y + std::get<Control*>(control)->position().y);
		}
		else
		{
			auto groupBoundary = std::get<ControlGroup*>(control)->calculateBoundary();
			boundary.position.x = std::min(boundary.position.x, groupBoundary.position.x);
			boundary.position.y = std::min(boundary.position.y, groupBoundary.position.y);
			boundary.size.x = std::max(boundary.size.x, groupBoundary.size.x);
			boundary.size.y = std::max(boundary.size.y, groupBoundary.size.y);
		}
	}
	return boundary;
}
