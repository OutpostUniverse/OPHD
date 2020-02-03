#pragma once

#include "Structure.h"


class SeedLander: public Structure
{
public:
	typedef NAS2D::Signals::Signal2<int, int> Callback;

public:
	SeedLander() = delete;
	SeedLander(int x, int y):	Structure(constants::SEED_LANDER, "structures/seed_0.sprite", CLASS_LANDER),
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

	virtual ~SeedLander() = default;

	void position(int x, int y)
	{
		mX = x;
		mY = y;
	}

	Callback& deployCallback() { return mDeploy; }

protected:
	virtual void think()
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
