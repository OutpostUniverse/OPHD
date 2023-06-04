#include "Common.h"
#include "Constants/Numbers.h"
#include "StructureManager.h"
#include "XmlSerializer.h"

#include "MapObjects/Structure.h"
#include "MapObjects/Structures/Warehouse.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Xml/XmlDocument.h>
#include <NAS2D/Xml/XmlElement.h>

#include <stdexcept>
#include <algorithm>


using namespace NAS2D;


namespace
{
	float meanSolarDistance = 1;
}


std::string difficultyString(Difficulty difficulty)
{
	for (const auto& difficultyPair : difficultyTable)
	{
		if (difficultyPair.second == difficulty)
		{
			return difficultyPair.first;
		}
	}

	throw std::runtime_error("Provided difficulty does not exist in the difficultyMap");
}


const std::map<StructureState, Color> STRUCTURE_COLOR_TABLE
{
	{StructureState::UnderConstruction, Color{150, 150, 150, 100}},
	{StructureState::Operational, Color{0, 185, 0}},
	{StructureState::Idle, Color{0, 185, 0, 100}},
	{StructureState::Disabled, Color{220, 0, 0}},
	{StructureState::Destroyed, Color{220, 0, 0}}
};


const std::map<StructureState, Color> STRUCTURE_TEXT_COLOR_TABLE
{
	{StructureState::UnderConstruction, Color{185, 185, 185, 100}},
	{StructureState::Operational, Color{0, 185, 0}},
	{StructureState::Idle, Color{0, 185, 0, 100}},
	{StructureState::Disabled, Color{220, 0, 0}},
	{StructureState::Destroyed, Color{220, 0, 0}}
};


const std::map<TerrainType, std::string> TILE_INDEX_TRANSLATION =
{
	{TerrainType::Dozed, constants::TileBulldozed},
	{TerrainType::Clear, constants::TileClear},
	{TerrainType::Rough, constants::TileRough},
	{TerrainType::Difficult, constants::TileDifficult},
	{TerrainType::Impassable, constants::TileImpassable},
};


const std::map<MineProductionRate, std::string> MINE_YIELD_TRANSLATION =
{
	{MineProductionRate::High, constants::MineYieldHigh},
	{MineProductionRate::Low, constants::MineYieldLow},
	{MineProductionRate::Medium, constants::MineYieldMedium}
};


const std::map<DisabledReason, std::string> DISABLED_REASON_TABLE =
{
	{DisabledReason::None, constants::StructureDisabledNone},

	{DisabledReason::Chap, constants::StructureDisabledChap},
	{DisabledReason::Disconnected, constants::StructureDisabledDisconnected},
	{DisabledReason::Energy, constants::StructureDisabledEnergy},
	{DisabledReason::Population, constants::StructureDisabledPopulation},
	{DisabledReason::RefinedResources, constants::StructureDisabledRefinedResources},
	{DisabledReason::StructuralIntegrity, constants::StructureDisabledStructuralIntegrity}
};


const std::map<IdleReason, std::string> IDLE_REASON_TABLE =
{
	{IdleReason::None, constants::StructureIdleNone},

	{IdleReason::PlayerSet, constants::StructureIdlePlayerSet},
	{IdleReason::InternalStorageFull, constants::StructureIdleInternalStorageFull},
	{IdleReason::FactoryProductionComplete, constants::StructureIdleFactoryProductionComplete},
	{IdleReason::FactoryInsufficientResources, constants::StructureIdleFactoryInsufficientResources},
	{IdleReason::FactoryInsufficientRobotCommandCapacity, constants::StructureIdleFactoryInsufficientRobotCommandCapacity},
	{IdleReason::FactoryInsufficientWarehouseSpace, constants::StructureIdleFactoryInsufficnetWarehouseCapacity},
	{IdleReason::MineExhausted, constants::StructureIdleMineExhausted},
	{IdleReason::MineInactive, constants::StructureIdleMineInactive},
	{IdleReason::InsufficientLuxuryProduct, constants::StructureIdleInsufficientLuxuryProduct}
};


const std::array MoraleStringTable =
{
	std::string("Terrible"),
	std::string("Poor"),
	std::string("Fair"),
	std::string("Good"),
	std::string("Excellent"),

	std::string("Morale is "),
	std::string("Births"),
	std::string("Deaths"),
	std::string("No active Food Production"),
	std::string("Parks & Arboretums"),
	std::string("Recreational Facilities"),
	std::string("Luxury Availability"),
	std::string("Residential Over Capacity"),
	std::string("Biowaste Overflowing"),
	std::string("Structures Disabled"),
	std::string("Structures Destroyed")
};


const std::string& moraleString(std::size_t index)
{
	return MoraleStringTable[index];
}


const std::string& moraleString(Morale morale)
{
	return MoraleStringTable[static_cast<std::size_t>(morale)];
}


std::size_t moraleStringTableCount()
{
	return MoraleStringTable.size();
}


const std::array<std::string, 4> ResourceNamesRefined =
{
	{"Common Metals", "Common Minerals", "Rare Metals", "Rare Minerals"}
};


const std::array<std::string, 4> ResourceNamesOre =
{
	{"Common Metals Ore", "Common Minerals Ore", "Rare Metals Ore", "Rare Minerals Ore"}
};


const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsRefined =
{
	NAS2D::Rectangle<int>{{64, 16}, {16, 16}},
	NAS2D::Rectangle<int>{{96, 16}, {16, 16}},
	NAS2D::Rectangle<int>{{80, 16}, {16, 16}},
	NAS2D::Rectangle<int>{{112, 16}, {16, 16}},
};


const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsOre =
{
	NAS2D::Rectangle<int>{{64, 0}, {16, 16}},
	NAS2D::Rectangle<int>{{96, 0}, {16, 16}},
	NAS2D::Rectangle<int>{{80, 0}, {16, 16}},
	NAS2D::Rectangle<int>{{112, 0}, {16, 16}},
};


const std::map<std::array<bool, 4>, std::string> IntersectionPatternTable =
{
	{{true, false, true, false}, "left"},
	{{true, false, false, false}, "left"},
	{{false, false, true, false}, "left"},

	{{false, true, false, true}, "right"},
	{{false, true, false, false}, "right"},
	{{false, false, false, true}, "right"},

	{{false, false, false, false}, "intersection"},
	{{true, true, false, false}, "intersection"},
	{{false, false, true, true}, "intersection"},
	{{false, true, true, true}, "intersection"},
	{{true, true, true, false}, "intersection"},
	{{true, true, true, true}, "intersection"},
	{{true, false, false, true}, "intersection"},
	{{false, true, true, false}, "intersection"},

	{{false, true, true, true}, "intersection"},
	{{true, false, true, true}, "intersection"},
	{{true, true, false, true}, "intersection"},
	{{true, true, true, false}, "intersection"}
};


const std::string& disabledReason(DisabledReason disabledReason)
{
	return DISABLED_REASON_TABLE.at(disabledReason);
}


const std::string& idleReason(IdleReason idleReason)
{
	return IDLE_REASON_TABLE.at(idleReason);
}


Color structureColorFromIndex(StructureState structureState)
{
	return STRUCTURE_COLOR_TABLE.at(structureState);
}


Color structureTextColorFromIndex(StructureState structureState)
{
	return STRUCTURE_TEXT_COLOR_TABLE.at(structureState);
}


void checkSavegameVersion(const std::string& filename)
{
	// openSavegame checks version number after opening file
	openSavegame(filename);
}


/**
 * Open a saved game and validate version.
 *
 * \throws	Throws a std::runtime_error if there are any errors with a savegame version, formation or missing root nodes.
 */
NAS2D::Xml::XmlDocument openSavegame(const std::string& filename)
{
	auto xmlDocument = openXmlFile(filename, constants::SaveGameRootNode);

	auto savegameVersion = xmlDocument.firstChildElement(constants::SaveGameRootNode)->attribute("version");

	if (savegameVersion != constants::SaveGameVersion)
	{
		throw std::runtime_error("Savegame version mismatch: '" + filename + "'. Expected " + constants::SaveGameVersion + ", found " + savegameVersion + ".");
	}

	return xmlDocument;
}


void setMeanSolarDistance(float newMeanSolarDistance)
{
	if (newMeanSolarDistance <= 0)
	{
		throw std::runtime_error("Must set a positive value for `meanSolarDistance`: " + std::to_string(newMeanSolarDistance));
	}
	meanSolarDistance = newMeanSolarDistance;
}


float getMeanSolarDistance()
{
	return meanSolarDistance;
}


void drawProgressBar(int value, int max, NAS2D::Rectangle<int> rect, int padding)
{
	const auto clippedValue = std::clamp(value, 0, max);
	auto& renderer = Utility<Renderer>::get();
	renderer.drawBox(rect, NAS2D::Color{0, 185, 0});

	if (max > 0)
	{
		auto innerRect = rect.inset(padding);
		innerRect.size.x *= clippedValue / max;
		renderer.drawBoxFilled(innerRect, NAS2D::Color{0, 100, 0});
	}
}


int getTruckAvailability()
{
	int trucksAvailable = 0;

	auto& warehouseList = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto warehouse : warehouseList)
	{
		trucksAvailable += warehouse->products().count(ProductType::PRODUCT_TRUCK);
	}

	return trucksAvailable;
}


int pullTruckFromInventory()
{
	int trucksAvailable = getTruckAvailability();

	if (trucksAvailable == 0) { return 0; }

	auto& warehouseList = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto warehouse : warehouseList)
	{
		if (warehouse->products().pull(ProductType::PRODUCT_TRUCK, 1) > 0)
		{
			return 1;
		}
	}

	return 0;
}


int pushTruckIntoInventory()
{
	const int storageNeededForTruck = storageRequiredPerUnit(ProductType::PRODUCT_TRUCK);

	auto& warehouseList = NAS2D::Utility<StructureManager>::get().getStructures<Warehouse>();
	for (auto warehouse : warehouseList)
	{
		if (warehouse->products().availableStorage() >= storageNeededForTruck)
		{
			warehouse->products().store(ProductType::PRODUCT_TRUCK, 1);
			return 1;
		}
	}

	return 0;
}
