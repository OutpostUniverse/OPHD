#pragma once


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

	const int MINIMUM_WINDOW_WIDTH = 1000;
	const int MINIMUM_WINDOW_HEIGHT = 700;

	const int ROBOT_COM_RANGE = 15;
	const int COMM_TOWER_BASE_RANGE = 10;
	const int LANDER_COM_RANGE = 5;

	enum class PlanetHostility
	{
		HOSTILITY_NONE,

		HOSTILITY_LOW,
		HOSTILITY_MEDIUM,
		HOSTILITY_HIGH
	};
}
