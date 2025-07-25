#pragma once


/**
 * Connector Direction.
 *
 * \note	CONNECTOR_INTERSECTION is intentionally set to '1' to prevent
 *			breaking changes with save files.
 */
enum class ConnectorDir
{
	CONNECTOR_INTERSECTION = 1,
	CONNECTOR_EAST_WEST,
	CONNECTOR_NORTH_SOUTH,
	CONNECTOR_VERTICAL // Functions as an intersection
};
