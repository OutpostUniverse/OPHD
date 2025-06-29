#pragma once

#include "../Structure.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Signal/Signal.h>


class Tile;


class ColonistLander : public Structure
{
public:
	using DeployDelegate = NAS2D::Delegate<void()>;
	using Signal = NAS2D::Signal<>;

public:
	ColonistLander(Tile* tile);

	void deployHandler(DeployDelegate newDeployHandler);

protected:
	void think() override;

private:
	Signal mDeploy;

	Tile* mTile;
};
