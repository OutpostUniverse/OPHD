#pragma once

#include "Structure.h"

#include "NAS2D/Renderer/Point.h"


class SeedLander: public Structure
{
public:
	using Callback = NAS2D::Signals::Signal<NAS2D::Point<int>>;

public:
	SeedLander() = delete;
	SeedLander(int x, int y) :
		Structure(constants::SEED_LANDER, "structures/seed_0.sprite", StructureClass::CLASS_LANDER),
		mPosition{x, y}
	{
		sprite().play(constants::STRUCTURE_STATE_CONSTRUCTION);
		maxAge(50);
		turnsToBuild(1);
		repairable(false);
		requiresCHAP(false);
		selfSustained(true);

		enable();
	}

	void position(int x, int y)
	{
		mPosition = {x, y};
	}

	Callback& deployCallback() { return mDeploy; }

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			// Logic guard, probably not necessary.
			if (mPosition == NAS2D::Point{0, 0}) { return; }

			mDeploy(mPosition);
		}
	}

private:
	Callback mDeploy;
	NAS2D::Point<int> mPosition;
};
