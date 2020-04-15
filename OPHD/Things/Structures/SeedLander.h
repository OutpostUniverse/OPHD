#pragma once

#include "Structure.h"


class SeedLander: public Structure
{
public:
	using Callback = NAS2D::Signals::Signal<int, int>;

public:
	SeedLander() = delete;
	SeedLander(int x, int y):	Structure(constants::SEED_LANDER, "structures/seed_0.sprite", StructureClass::CLASS_LANDER),
								mX(x), mY(y)
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
		mX = x;
		mY = y;
	}

	Callback& deployCallback() { return mDeploy; }

protected:
	void think() override
	{
		if (age() == turnsToBuild())
		{
			// Logic guard, probably not necessary.
			if (mX == 0 && mY == 0) { return; }

			mDeploy(mX, mY);
		}
	}

private:
	Callback	mDeploy;

	int			mX = 0, mY = 0;
};
