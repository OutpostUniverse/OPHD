#pragma once

#include "../Structure.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Signal/Signal.h>


class Tile;


class SeedLander : public Structure
{
public:
	using DeployDelegate = NAS2D::Delegate<void(NAS2D::Point<int>)>;
	using Signal = NAS2D::Signal<NAS2D::Point<int>>;

public:
	SeedLander() = delete;
	SeedLander(const Tile* tile);

	void deployHandler(DeployDelegate newDeployHandler);

protected:
	void think() override;

private:
	Signal mDeploy;
	NAS2D::Point<int> mPosition;
};
