// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "RobotInspector.h"
#include "TextRender.h"

#include "../Cache.h"
#include "../Constants.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


namespace
{
	const NAS2D::Image& robotImage(RobotType robotType)
	{
		static const std::map<RobotType, const Image*> robotImages
		{
			{RobotType::Digger, &imageCache.load("ui/interface/product_robodigger.png")},
			{RobotType::Dozer, &imageCache.load("ui/interface/product_robodozer.png")},
			{RobotType::Miner, &imageCache.load("ui/interface/product_robominer.png")}
		};
		return *robotImages.at(robotType);
	}
}


RobotInspector::RobotInspector() :
	mFont{ fontCache.load(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL) },
	mFontBold{ fontCache.load(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL) },
	btnCancelOrders{ "Cancel Orders" },
	btnSelfDestruct{ "Self Destruct" },
	btnIdle{ "Set Idle" },
	btnCancel{ constants::ButtonCancel }
{
	init();
}


void RobotInspector::init()
{
	constexpr int padding = constants::MARGIN * 2;
	const int buttonWidth = mFont.width("Cancel Orders") + padding;
	const int buttonHeight = mFont.height() + padding;

	const int contentHeight = (buttonHeight + constants::MARGIN) * 4;
	const int imgHeight = robotImage(RobotType::Digger).size().y + padding;

	const int largestHeight = imgHeight > contentHeight ? imgHeight : contentHeight;

	size({ buttonWidth + robotImage(RobotType::Digger).size().x + padding + constants::MARGIN, 300 });

	add(btnCancelOrders, { 0, 0 });
	btnCancelOrders.size({ buttonWidth, buttonHeight });

	add(btnSelfDestruct, { 0, 0 });
	btnSelfDestruct.size({ buttonWidth, buttonHeight });

	add(btnIdle, { 0, 0 });
	btnIdle.size({ buttonWidth, buttonHeight });

	add(btnCancel, { 0, 0 });
	btnCancel.size({ mFont.width(constants::ButtonCancel), buttonHeight });
}


void RobotInspector::focusOnRobot(Robot* robot)
{
	if (!robot) { throw std::runtime_error("RobotInspector::focusOnRobot(): nullptr passed "); }

	mRobot = robot;
	text(robot->name());
}


void RobotInspector::update()
{
	if (!visible()) { return; }
	Window::update();

	auto& renderer = Utility<Renderer>::get();

	const auto labelPosition = Point{ btnCancelOrders.positionX(), positionY() + sWindowTitleBarHeight + constants::MARGIN };
	const auto labelWidth = btnCancelOrders.rect().width + (constants::MARGIN * 2);

	drawLabelAndValueRightJustify(labelPosition, labelWidth, "Something Something Robot", "Value");
}
