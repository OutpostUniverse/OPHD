#include "RobotDeploymentSummary.h"

#include "../RobotPool.h"
#include "../Cache.h"
#include "../Constants/UiConstants.h"
#include "../MapObjects/RobotType.h"
#include "../States/MapViewStateHelper.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>

#include <array>


RobotDeploymentSummary::RobotDeploymentSummary(const RobotPool& robotPool) :
	mRobotPool{robotPool},
	mUiIcons{imageCache.load("ui/icons.png")}
{}


void RobotDeploymentSummary::draw(NAS2D::Renderer& renderer) const
{
	if (!isCcPlaced()) { return; }

	auto position = mRect.position;
	constexpr auto textOffset = NAS2D::Vector{30, 7};

	const auto minerImageRect = NAS2D::Rectangle<int>{{231, 18}, {25, 25}};
	const auto dozerImageRect = NAS2D::Rectangle<int>{{206, 18}, {25, 25}};
	const auto diggerImageRect = NAS2D::Rectangle<int>{{181, 18}, {25, 25}};
	const auto robotSummaryImageRect = NAS2D::Rectangle<int>{{231, 43}, {25, 25}};

	const std::array icons{
		std::tuple{robotSummaryImageRect, mRobotPool.currentControlCount(), mRobotPool.robotControlMax()},
		std::tuple{diggerImageRect, mRobotPool.getAvailableCount(RobotType::Digger), mRobotPool.diggers().size()},
		std::tuple{dozerImageRect, mRobotPool.getAvailableCount(RobotType::Dozer), mRobotPool.dozers().size()},
		std::tuple{minerImageRect, mRobotPool.getAvailableCount(RobotType::Miner), mRobotPool.miners().size()},
	};

	for (const auto& [imageRect, parts, total] : icons)
	{
		renderer.drawSubImage(mUiIcons, position, imageRect);
		const auto text = std::to_string(parts) + " / " + std::to_string(total);
		renderer.drawText(Control::getDefaultFont(), text, position + textOffset, NAS2D::Color::White);
		position.y += 25;
	}
}
