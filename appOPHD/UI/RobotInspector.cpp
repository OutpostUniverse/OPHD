#include "RobotInspector.h"
#include "TextRender.h"

#include "../CacheImage.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/Robot.h"
#include "../MapObjects/RobotType.h"

#include <NAS2D/Renderer/Renderer.h>
#include <NAS2D/Resource/Font.h>

#include <array>
#include <algorithm>
#include <stdexcept>


namespace
{
	const NAS2D::Image& robotImage(RobotTypeIndex robotTypeIndex)
	{
		static const std::array robotImages
		{
			&imageCache.load(Robot::robotType(RobotTypeIndex::Digger).imagePath),
			&imageCache.load(Robot::robotType(RobotTypeIndex::Dozer).imagePath),
			&imageCache.load(Robot::robotType(RobotTypeIndex::Miner).imagePath),
		};
		return *robotImages.at(static_cast<std::size_t>(robotTypeIndex));
	}
}


RobotInspector::RobotInspector() :
	btnCancelOrders{"Cancel Orders", {this, &RobotInspector::onCancelOrders}},
	btnSelfDestruct{"Self Destruct", {this, &RobotInspector::onSelfDestruct}},
	btnCancel{constants::Cancel, {this, &RobotInspector::onCancel}}
{
	const NAS2D::Font& mainFont = Control::getDefaultFont();
	const NAS2D::Font& mainFontBold = Control::getDefaultFontBold();

	constexpr int padding = constants::Margin * 2;
	const auto buttonSize = mainFont.size("Cancel Orders") + NAS2D::Vector{padding, padding};
	const int buttonOffsetY = buttonSize.y + constants::MarginTight;

	const int imageWidth = robotImage(RobotTypeIndex::Digger).size().x + padding;
	const int contentWidth = imageWidth + buttonSize.x;

	mContentRect = {
		{imageWidth,
		sWindowTitleBarHeight + constants::Margin},
		{std::max(mainFontBold.width("Age") + mainFont.width("    9999"), buttonSize.x),
		mainFont.height() + constants::Margin}
	};

	auto buttonPosition = NAS2D::Vector{imageWidth,  mContentRect.position.y + mContentRect.size.y + constants::Margin};

	btnCancelOrders.size(buttonSize);
	add(btnCancelOrders, buttonPosition);
	buttonPosition.y += buttonOffsetY;

	btnSelfDestruct.size(buttonSize);
	add(btnSelfDestruct, buttonPosition);

	btnCancel.size({mainFont.width(constants::Cancel) + padding, buttonSize.y});
	buttonPosition = {contentWidth - btnCancel.size().x, buttonPosition.y + buttonOffsetY * 2};
	add(btnCancel, buttonPosition);

	size({contentWidth + constants::Margin, buttonPosition.y + buttonSize.y + constants::Margin});
}


void RobotInspector::focusOnRobot(Robot* robot)
{
	if (!robot) { throw std::runtime_error("RobotInspector::focusOnRobot(): nullptr passed "); }

	mRobot = robot;
	title(robot->name());
}


void RobotInspector::onCancelOrders()
{
	mRobot->cancelTask();
	hide();
}


void RobotInspector::onSelfDestruct()
{
	mRobot->selfDestruct(true);
	hide();
}


void RobotInspector::onCancel()
{
	hide();
}


void RobotInspector::drawClientArea(NAS2D::Renderer& renderer) const
{
	renderer.drawImage(robotImage(mRobot->type()), position() + NAS2D::Vector{constants::Margin, constants::Margin + sWindowTitleBarHeight});

	const auto labelPosition = area().position + NAS2D::Vector{mContentRect.position.x, mContentRect.position.y};
	drawLabelAndValueRightJustify(labelPosition, mContentRect.size.x, "Age", std::to_string(mRobot->fuelCellAge()));
}
