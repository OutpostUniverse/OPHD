#pragma once

#include "../Robot.h"

#include <libOPHD/Map/MapCoordinate.h>

enum class Direction;


class Robodigger : public Robot
{
public:
	Robodigger();

	void direction(Direction dir);
	Direction direction() const;

	void target(const MapCoordinate& target);
	const MapCoordinate& target() const;

	NAS2D::Dictionary getDataDict() const override;

protected:
	void onTaskComplete(TileMap& tileMap, StructureManager& structureManager) override;

private:
	Direction mDirection;
	MapCoordinate mTarget;
};
