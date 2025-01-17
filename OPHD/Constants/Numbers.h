#pragma once

#include <NAS2D/Math/Vector.h>


/**
 * Numeric constants
 */
namespace constants
{
	inline constexpr int PlanetAnimationSpeed{35};

	inline constexpr int SmeltingMinimumResourcesCount{20};

	inline constexpr int BaseStorageCapacity{250};

	inline constexpr int BaseProductCapacity{100};

	inline constexpr int BaseMineProductionRate{10};
	inline constexpr int BaseMineExtensionTime{10};

	/**<
	 * The number of times a truck can traverse the shortest possible path
	 * between a mine and a smelter (adjacent to one another). A truck can move 1
	 * unit of ore per trip. The shortest path cost is 0.50f. This number
	 * represents 100 round trips between the mine/smelter for effectively 100
	 * units of ore transported per turn.
	 */
	inline constexpr float ShortestPathTraversalCount{100.0f};

	inline constexpr int ColonyShipOrbitTime{24};

	inline constexpr int MinerTaskTime{6};

	inline constexpr int DiggerTaskTime{5};

	inline constexpr int CommandCenterPopulationCapacity{10};

	inline constexpr int RobotCommandCapacity{10};

	inline constexpr auto MinimumWindowSize{NAS2D::Vector{1000, 700}};

	inline constexpr int RobotCommRange{15};
	inline constexpr int LanderCommRange{5};

	inline constexpr int RoadIntegrityChange{80};

	inline constexpr float RouteBaseCost{0.5f};
	inline constexpr float RouteRoadCost{0.25f};
}
