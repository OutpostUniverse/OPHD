#pragma once


/**
 * Connector Direction.
 *
 * \note	CONNECTOR_INTERSECTION is intentionally set to '1' to prevent
 *			breaking changes with save files.
 */
enum ConnectorDir
{
	CONNECTOR_INTERSECTION = 1,
	CONNECTOR_RIGHT,
	CONNECTOR_LEFT,
	CONNECTOR_VERTICAL // Functions as an intersection
};
