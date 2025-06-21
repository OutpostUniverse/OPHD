#pragma once

#include "../Structure.h"

#include <NAS2D/Math/Point.h>
#include <NAS2D/Signal/Signal.h>


class Tile;


class SeedLander : public Structure
{
public:
	using Signal = NAS2D::Signal<NAS2D::Point<int>>;

public:
	SeedLander() = delete;
	SeedLander(const Tile* tile);

	Signal::Source& deploySignal();

protected:
	void think() override;

private:
	Signal mDeploy;
	NAS2D::Point<int> mPosition;
};
