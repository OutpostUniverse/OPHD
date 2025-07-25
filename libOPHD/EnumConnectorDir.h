#pragma once


/**
 * Connector Direction.
 *
 * \note	Intersection is intentionally set to '1' to prevent
 *			breaking changes with save files.
 */
enum class ConnectorDir
{
	Intersection = 1,
	EastWest,
	NorthSouth,
	Vertical // Functions as an intersection
};
