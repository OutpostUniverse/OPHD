#pragma once

#include "../Structure.h"

#include <NAS2D/Signal/Delegate.h>


class Tile;


class ColonistLander : public Structure
{
public:
	using DeployDelegate = NAS2D::Delegate<void()>;

public:
	ColonistLander(Tile& tile);

	void deployHandler(DeployDelegate newDeployHandler);

protected:
	void think() override;

private:
	DeployDelegate mDeployHandler;

	Tile* mTile;
};
