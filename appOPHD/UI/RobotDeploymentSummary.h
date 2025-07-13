#pragma once

#include <libControls/Control.h>


namespace NAS2D
{
	class Image;
}

class RobotPool;


class RobotDeploymentSummary : public Control
{
public:
	RobotDeploymentSummary(const RobotPool& robotPool);

	void draw(NAS2D::Renderer& renderer) const override;

private:
	const RobotPool& mRobotPool;
	const NAS2D::Image& mUiIcons;
};
