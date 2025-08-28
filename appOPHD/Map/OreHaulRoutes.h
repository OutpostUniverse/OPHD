#pragma once

#include "RouteFinder.h"

#include <vector>


struct Route;
class Tile;
class TileMap;
class MineFacility;
class StructureManager;


class OreHaulRoutes
{
public:
	OreHaulRoutes(TileMap& tileMap, const StructureManager& structureManager);

	bool hasRoute(const MineFacility& mineFacility) const;
	const Route& getRoute(const MineFacility& mineFacility) const;
	float getRouteCost(const MineFacility& mineFacility) const;
	int getOreHaulCapacity(const MineFacility& mineFacility) const;

	void removeMine(const MineFacility& mineFacility);
	void clear();

	void updateRoutes();
	void transportOreFromMines();

	std::vector<Tile*> getRouteOverlay() const;

private:
	RouteFinder mRouteFinder;
	const StructureManager& mStructureManager;
};
