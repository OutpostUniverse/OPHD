#pragma once

#include "Structure.h"

#include <NAS2D/Renderer/Point.h>


class SeedLander: public Structure
{
public:
	using Callback = NAS2D::Signal<NAS2D::Point<int>>;

public:
	SeedLander() = delete;
	SeedLander(NAS2D::Point<int> position) :
		Structure{constants::SEED_LANDER,
		"structures/seed_0.sprite",
		StructureClass::Lander,
		StructureID::SID_SEED_LANDER},
		mPosition{position}
	{
		maxAge(50);
		turnsToBuild(1);
		repairable(false);
		requiresCHAP(false);
		selfSustained(true);

		enable();
	}

	void position(NAS2D::Point<int> position)
	{
		mPosition = position;
	}

	Callback::Source& deployCallback() { return mDeploy; }

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
