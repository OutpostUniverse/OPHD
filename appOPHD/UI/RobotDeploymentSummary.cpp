#include "RobotDeploymentSummary.h"

#include "../RobotPool.h"
#include "../Cache.h"
#include "../MapObjects/RobotTypeIndex.h"

#include <NAS2D/Renderer/Renderer.h>

#include <tuple>


RobotDeploymentSummary::RobotDeploymentSummary(const RobotPool& robotPool) :
	mRobotPool{robotPool},
	mUiIcons{imageCache.load("ui/icons.png")}
{}


void RobotDeploymentSummary::draw(NAS2D::Renderer& renderer) const
{
	if (mRobotPool.currentControlCount() == 0 && mRobotPool.robotControlMax() == 0) { return; }

	auto position = mRect.position;
	constexpr auto textOffset = NAS2D::Vector{30, 7};

	const auto robotCommandImageRect = NAS2D::Rectangle<int>{{231, 43}, {25, 25}};
	const auto diggerImageRect = NAS2D::Rectangle<int>{{181, 18}, {25, 25}};
	const auto dozerImageRect = NAS2D::Rectangle<int>{{206, 18}, {25, 25}};
	const auto minerImageRect = NAS2D::Rectangle<int>{{231, 18}, {25, 25}};

	const auto icons = {
		std::tuple{robotCommandImageRect, mRobotPool.currentControlCount(), mRobotPool.robotControlMax()},
		std::tuple{diggerImageRect, mRobotPool.getAvailableCount(RobotTypeIndex::Digger), mRobotPool.diggers().size()},
		std::tuple{dozerImageRect, mRobotPool.getAvailableCount(RobotTypeIndex::Dozer), mRobotPool.dozers().size()},
		std::tuple{minerImageRect, mRobotPool.getAvailableCount(RobotTypeIndex::Miner), mRobotPool.miners().size()},
	};

	for (const auto& [imageRect, parts, total] : icons)
	{
		renderer.drawSubImage(mUiIcons, position, imageRect);
		const auto text = std::to_string(parts) + " / " + std::to_string(total);
		renderer.drawText(Control::getDefaultFont(), text, position + textOffset, NAS2D::Color::White);
		position.y += 25;
	}
}
