#include "RobotInspector.h"
#include "TextRender.h"

#include "../Cache.h"
#include "../Constants/Strings.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/Robot.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <algorithm>


using namespace NAS2D;


namespace
{
	const NAS2D::Image& robotImage(RobotTypeIndex robotType)
	{
		static const std::map<RobotTypeIndex, const Image*> robotImages
		{
			{RobotTypeIndex::Digger, &imageCache.load("ui/interface/product_robodigger.png")},
			{RobotTypeIndex::Dozer, &imageCache.load("ui/interface/product_robodozer.png")},
			{RobotTypeIndex::Miner, &imageCache.load("ui/interface/product_robominer.png")}
		};
		return *robotImages.at(robotType);
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

	auto buttonPosition = Vector{imageWidth,  mContentRect.position.y + mContentRect.size.y + constants::Margin};

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


void RobotInspector::drawClientArea() const
{
	auto& renderer = Utility<Renderer>::get();
	renderer.drawImage(robotImage(mRobot->type()), position() + Vector{constants::Margin, constants::Margin + sWindowTitleBarHeight});

	const auto labelPosition = area().position + Vector{mContentRect.position.x, mContentRect.position.y};
	drawLabelAndValueRightJustify(labelPosition, mContentRect.size.x, "Age", std::to_string(mRobot->fuelCellAge()));
}
