#pragma once

#include "../Structure.h"

#include <NAS2D/Signal/Delegate.h>
#include <NAS2D/Signal/Signal.h>


class Tile;


class CargoLander : public Structure
{
public:
	using DeployDelegate = NAS2D::Delegate<void()>;
	using Signal = NAS2D::Signal<>;

	CargoLander(Tile* tile);

	void deployHandler(DeployDelegate newDeployHandler);

protected:
	void think() override;

private:
	CargoLander() = delete;
	CargoLander(const CargoLander&) = delete;
	CargoLander& operator=(const CargoLander&) = delete;

private:
	Signal mDeploy;
	Tile* mTile = nullptr;
};
