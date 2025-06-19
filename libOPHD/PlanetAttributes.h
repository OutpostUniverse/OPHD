#pragma once

#include <cstddef>
#include <string>
#include <vector>


enum class PlanetType
{
	Mercury,
	Mars,
	Ganymede,
};


enum class PlanetHostility
{
	Low,
	Medium,
	High
};


struct PlanetAttributes
{
	PlanetType type{PlanetType::Mercury};
	std::string imagePath;
	PlanetHostility hostility{PlanetHostility::Low};
	int maxDepth = 0;
	std::size_t maxOreDeposits = 0;
	std::string mapImagePath;
	std::string tilesetPath;
	std::string name;

	/* Mean distance from star in astronomical units (AU) */
	float meanSolarDistance = 0;

	std::string description;
};


std::vector<PlanetAttributes> parsePlanetAttributes(const std::string& filename);
