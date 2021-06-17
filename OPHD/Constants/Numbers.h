#pragma once

#include <NAS2D/Renderer/Vector.h>


/**
 * Numeric constants
 */
namespace constants
{
	inline constexpr int FADE_SPEED{ 300 };

	inline constexpr int PLANET_ANIMATION_SPEED{ 50 };

	inline constexpr int MINIMUM_RESOURCES_REQUIRE_FOR_SMELTING{ 20 };

	inline constexpr int BASE_STORAGE_CAPACITY{ 250 };

	inline constexpr int BASE_PRODUCT_CAPACITY{ 100 };

	inline constexpr int BASE_MINE_PRODUCTION_RATE{ 10 };
	inline constexpr int BASE_MINE_SHAFT_EXTENSION_TIME{ 10 };

	/**<
	 * The number of times a truck can traverse the shortest possible path
	 * between a mine and a smelter (adjacent to one another). A truck can move 1
	 * unit of ore per trip. The shortest path cost is 0.50f. This number
	 * represents 100 round trips between the mine/smelter for effectively 100
	 * units of ore transported per turn.
	 */
	inline constexpr float ShortestPathTraversalCount{ 100.0f };

	inline constexpr int DEPTH_SURFACE{ 0 };
	inline constexpr int DEPTH_UNDERGROUND_1{ 1 };
	inline constexpr int DEPTH_UNDERGROUND_2{ 2 };
	inline constexpr int DEPTH_UNDERGROUND_3{ 3 };
	inline constexpr int DEPTH_UNDERGROUND_4{ 4 };

	inline constexpr int COLONY_SHIP_ORBIT_TIME{ 24 };

	inline constexpr int MINER_TASK_TIME{ 6 };

	inline constexpr int DIGGER_TASK_TIME{ 5 };

	inline constexpr int COMMAND_CENTER_POPULATION_CAPACITY{ 10 };
	inline constexpr int MINIMUM_RESIDENCE_OVERCAPACITY_HIT{ 1 };

	inline constexpr int ROBOT_COMMAND_CAPACITY{ 10 };

	inline constexpr int DEFAULT_STARTING_MORALE{ 600 };

	inline constexpr auto MinimumWindowSize{ NAS2D::Vector{1000, 700} };

	inline constexpr int ROBOT_COM_RANGE{ 15 };
	inline constexpr int LANDER_COM_RANGE{ 5 };

	inline constexpr float ROUTE_BASE_COST{ 0.5f };
	inline constexpr float ROUTE_ROAD_COST{ 0.25 };
}
