#pragma once

#include "Structure.h"

#include "../../Constants.h"
#include "../../Map/Tile.h"

class CargoLander : public Structure
{
public:

	using Callback = NAS2D::Signals::Signal<>;

	CargoLander(Tile* t) : Structure(constants::CARGO_LANDER, "structures/lander_0.sprite", StructureClass::CLASS_LANDER), mTile(t)
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(50);
		turnsToBuild(1);
		repairable(false);
		requiresCHAP(false);
		selfSustained(true);

		enable();
	}

	Callback& deployCallback() { return mDeploy; }

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			mDeploy();
			mTile->index(0);
		}
	}

private:
	CargoLander() = delete;
	CargoLander(const CargoLander&) = delete;
	CargoLander& operator=(const CargoLander&) = delete;

private:
	Callback mDeploy;
	Tile* mTile = nullptr;
};
