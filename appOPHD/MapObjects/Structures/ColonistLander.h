#pragma once

#include "../Structure.h"


class Tile;


class ColonistLander : public Structure
{
public:
	using Signal = NAS2D::Signal<>;

public:
	ColonistLander(Tile* tile);

	Signal::Source& deploySignal();

protected:
	void think() override;

private:
	Signal mDeploy;

	Tile* mTile;
};
