#include "RobotInspector.h"
#include "TextRender.h"

#include "../Cache.h"
#include "../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


namespace
{
	const NAS2D::Image& robotImage(Robot::Type robotType)
	{
		static const std::map<Robot::Type, const Image*> robotImages
		{
			{Robot::Type::Digger, &imageCache.load("ui/interface/product_robodigger.png")},
			{Robot::Type::Dozer, &imageCache.load("ui/interface/product_robodozer.png")},
			{Robot::Type::Miner, &imageCache.load("ui/interface/product_robominer.png")}
		};
		return *robotImages.at(robotType);
	}
}


RobotInspector::RobotInspector() :
	btnCancelOrders{ "Cancel Orders", {this, &RobotInspector::onCancelOrders} },
	btnSelfDestruct{ "Self Destruct", {this, &RobotInspector::onSelfDestruct} },
	btnCancel{ constants::ButtonCancel, {this, &RobotInspector::onCancel} }
{
	const NAS2D::Font& mainFont = fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	const NAS2D::Font& mainFontBold = fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);

	constexpr int padding = constants::MARGIN * 2;
	const auto buttonSize = mainFont.size("Cancel Orders") + NAS2D::Vector{padding, padding};
	const int buttonOffsetY = buttonSize.y + constants::MARGIN_TIGHT;

	const int imageWidth = robotImage(Robot::Type::Digger).size().x + padding;
	const int contentWidth = imageWidth + buttonSize.x;

	mContentArea = {
		imageWidth,
		sWindowTitleBarHeight + constants::MARGIN,
		mainFontBold.width("Age") + mainFont.width("    9999"),
		mainFont.height() + constants::MARGIN
	};

	if (mContentArea.width < buttonSize.x) { mContentArea.width = buttonSize.x; }

	auto buttonPosition = Vector{ imageWidth,  mContentArea.y + mContentArea.height + constants::MARGIN };

	btnCancelOrders.size(buttonSize);
	add(btnCancelOrders, buttonPosition);
	buttonPosition.y += buttonOffsetY;

	btnSelfDestruct.size(buttonSize);
	add(btnSelfDestruct, buttonPosition);

	btnCancel.size({ mainFont.width(constants::ButtonCancel) + padding, buttonSize.y });
	buttonPosition = { contentWidth - btnCancel.size().x, buttonPosition.y + buttonOffsetY * 2 };
	add(btnCancel, buttonPosition);

	size({ contentWidth + constants::MARGIN, buttonPosition.y + buttonSize.y + constants::MARGIN });
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
	mRobot->seldDestruct(true);
	hide();
}


void RobotInspector::onCancel()
{
	hide();
}


void RobotInspector::update()
{
	if (!visible()) { return; }
	Window::update();

	auto& renderer = Utility<Renderer>::get();
	renderer.drawImage(robotImage(mRobot->type()), position() + Vector{ constants::MARGIN, constants::MARGIN + sWindowTitleBarHeight });

	const auto labelPosition = rect().startPoint() + Vector{ mContentArea.x, mContentArea.y };
	drawLabelAndValueRightJustify(labelPosition, mContentArea.width, "Age", std::to_string(mRobot->fuelCellAge()));
}
