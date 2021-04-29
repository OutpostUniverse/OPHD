#pragma once

#include <NAS2D/Renderer/Vector.h>


/**
 * Numeric constants
 */
namespace constants
{
	const int FADE_SPEED = 300;

	const int PLANET_ANIMATION_SPEED = 50;

	const int MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING = 20;

	const int BASE_STORAGE_CAPACITY = 250;

	const int BASE_PRODUCT_CAPACITY = 100;

	const int BASE_MINE_PRODUCTION_RATE = 10;
	const int BASE_MINE_SHAFT_EXTENSION_TIME = 10;

	/**<
	 * The number of times a truck can traverse the shortest possible path
	 * between a mine and a smelter (adjacent to one another). A truck can move 1
	 * unit of ore per trip. The shortest path cost is 0.50f. This number
	 * represents 100 round trips between the mine/smelter for effectively 100
	 * units of ore transported per turn.
	 */
	const float ShortestPathTraversalCount = 100.0f;

	const int DEPTH_SURFACE = 0;
	const int DEPTH_UNDERGROUND_1 = 1;
	const int DEPTH_UNDERGROUND_2 = 2;
	const int DEPTH_UNDERGROUND_3 = 3;
	const int DEPTH_UNDERGROUND_4 = 4;

	const int COLONY_SHIP_ORBIT_TIME = 24;

	const int MINER_TASK_TIME = 6;

	const int DIGGER_TASK_TIME = 5;

	const int COMMAND_CENTER_POPULATION_CAPACITY = 10;
	const int MINIMUM_RESIDENCE_OVERCAPACITY_HIT = 1;

	const int ROBOT_COMMAND_CAPACITY = 10;

	const int DEFAULT_STARTING_MORALE = 600;

	const auto MinimumWindowSize = NAS2D::Vector{1000, 700};

	const int ROBOT_COM_RANGE = 15;
	const int COMM_TOWER_BASE_RANGE = 10;
	const int LANDER_COM_RANGE = 5;

	const float ROUTE_BASE_COST = 0.5f;
	const float ROUTE_ROAD_COST = 0.25;
}
