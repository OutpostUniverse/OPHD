#pragma once

#include "../Structure.h"

#include "../../Map/Tile.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Signal/Signal.h>


class SeedLander : public Structure
{
public:
	using Signal = NAS2D::Signal<NAS2D::Point<int>>;

public:
	SeedLander() = delete;
	SeedLander(const Tile* tile) :
		Structure{
			StructureClass::Lander,
			StructureID::SID_SEED_LANDER
		},
		mPosition{tile->xy()}
	{
		enable();
	}

	void position(NAS2D::Point<int> position)
	{
		mPosition = position;
	}

	Signal::Source& deploySignal() { return mDeploy; }

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
	Signal mDeploy;
	NAS2D::Point<int> mPosition;
};
