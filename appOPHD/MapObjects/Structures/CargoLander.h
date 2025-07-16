#pragma once

#include "../Structure.h"

#include <NAS2D/Signal/Delegate.h>


class Tile;


class CargoLander : public Structure
{
public:
	using DeployDelegate = NAS2D::Delegate<void()>;

	CargoLander(Tile& tile);

	void deployHandler(DeployDelegate newDeployHandler);

protected:
	void think() override;

private:
	CargoLander() = delete;
	CargoLander(const CargoLander&) = delete;
	CargoLander& operator=(const CargoLander&) = delete;

private:
	DeployDelegate mDeployHandler;
	Tile* mTile = nullptr;
};
