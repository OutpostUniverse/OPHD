#include "RobotDeploymentSummary.h"

#include "../RobotPool.h"
#include "../Cache.h"
#include "../Constants/UiConstants.h"
#include "../States/MapViewStateHelper.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


extern const NAS2D::Font* MAIN_FONT; /// yuck


RobotDeploymentSummary::RobotDeploymentSummary(const RobotPool& robotPool) :
	mRobotPool{robotPool},
	mUiIcons{imageCache.load("ui/icons.png")}
{}


/**
 * Draws robot deployment information.
 */
void RobotDeploymentSummary::draw() const
{
	if (ccLocation() == CcNotPlaced) { return; }

	auto& renderer = NAS2D::Utility<NAS2D::Renderer>::get();

	// Robots: Miner (last one), Dozer (middle one), Digger (first one)
	// Start from the bottom - The bottom UI Height - Icons Height - 8 (1 offset to avoid the last to be glued with at the border)
	auto position = NAS2D::Point{8, renderer.size().y - constants::BottomUiHeight - 25 - 8};
	constexpr auto textOffset = NAS2D::Vector{30, 7};

	const auto minerImageRect = NAS2D::Rectangle{231, 18, 25, 25};
	const auto dozerImageRect = NAS2D::Rectangle{206, 18, 25, 25};
	const auto diggerImageRect = NAS2D::Rectangle{181, 18, 25, 25};
	const auto robotSummaryImageRect = NAS2D::Rectangle{231, 43, 25, 25};

	const std::array icons{
		std::tuple{minerImageRect, mRobotPool.getAvailableCount(Robot::Type::Miner), mRobotPool.miners().size()},
		std::tuple{dozerImageRect, mRobotPool.getAvailableCount(Robot::Type::Dozer), mRobotPool.dozers().size()},
		std::tuple{diggerImageRect, mRobotPool.getAvailableCount(Robot::Type::Digger), mRobotPool.diggers().size()},
		std::tuple{robotSummaryImageRect, mRobotPool.currentControlCount(), mRobotPool.robotControlMax()},
	};

	for (const auto& [imageRect, parts, total] : icons)
	{
		renderer.drawSubImage(mUiIcons, position, imageRect);
		const auto text = std::to_string(parts) + "/" + std::to_string(total);
		renderer.drawText(*MAIN_FONT, text, position + textOffset, NAS2D::Color::White);
		position.y -= 25;
	}
}
