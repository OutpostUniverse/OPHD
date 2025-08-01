#pragma once

#include "../Structure.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Signal/Delegate.h>


class SeedLander : public Structure
{
public:
	using DeployDelegate = NAS2D::Delegate<void(NAS2D::Point<int>)>;

public:
	SeedLander() = delete;
	SeedLander(Tile& tile);

	void deployHandler(DeployDelegate newDeployHandler);

protected:
	void think() override;

private:
	DeployDelegate mDeployHandler;
};
