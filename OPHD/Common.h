#pragma once

#include "EnumConnectorDir.h"

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Color.h>

#include <array>
#include <map>
#include <string>
#include <vector>

namespace NAS2D::Xml
{
	class XmlDocument;
}

enum class StructureState;

enum class Difficulty
{
	Beginner,
	Easy,
	Medium,
	Hard,
};


inline const std::map<std::string, Difficulty> difficultyTable
{
	{"Beginner", Difficulty::Beginner},
	{"Easy", Difficulty::Easy},
	{"Medium", Difficulty::Medium},
	{"Hard", Difficulty::Hard}
};


std::string difficultyString(Difficulty difficulty);


/**
 * Terrain type enumeration
 */
enum class TerrainType
{
	Dozed,
	Clear,
	Rough,
	Difficult,
	Impassable
};


enum class MineProductionRate
{
	Low,
	Medium,
	High,
};


enum class DisabledReason
{
	None, /**< Not Disabled, default reason. */

	Chap, /**< Requires atmosphere, no atmosphere available. */
	Disconnected, /**< Not connected to Command Center */
	Energy, /**< Not enough Energy to operate. */
	Population, /**< Insufficient workers or scientists (or both) */
	RefinedResources, /**< Insufficient mined and refined resources */
	StructuralIntegrity /**< Structural integrity out of operating tolerances (damaged structure) */
};


enum class IdleReason
{
	None,

	PlayerSet,
	InternalStorageFull,
	FactoryProductionComplete,
	FactoryInsufficientResources,
	FactoryInsufficientRobotCommandCapacity,
	FactoryInsufficientWarehouseSpace,
	MineExhausted,
	MineInactive,
	InsufficientLuxuryProduct
};


/**
 * Unique identifier code for each structure.
 *
 * \note	Each individual structure is identified using a SID_ code as opposed
 *			the structure Class code which is used to group like structures into
 *			lists for structure updates.
 */
enum StructureID
{
	SID_NONE,

	SID_AGRIDOME,
	SID_AIR_SHAFT,
	SID_CARGO_LANDER,
	SID_CHAP,
	SID_COLONIST_LANDER,
	SID_COMMAND_CENTER,
	SID_COMMERCIAL,
	SID_COMM_TOWER,
	SID_FUSION_REACTOR,
	SID_HOT_LABORATORY,
	SID_LABORATORY,
	SID_MEDICAL_CENTER,
	SID_MINE_FACILITY,
	SID_MINE_SHAFT,
	SID_NURSERY,
	SID_PARK,
	SID_RECREATION_CENTER,
	SID_RED_LIGHT_DISTRICT,
	SID_RESIDENCE,
	SID_ROAD,
	SID_ROBOT_COMMAND,
	SID_SEED_FACTORY,
	SID_SEED_LANDER,
	SID_SEED_POWER,
	SID_SEED_SMELTER,
	SID_SMELTER,
	SID_SOLAR_PANEL1,
	SID_SOLAR_PLANT,
	SID_STORAGE_TANKS,
	SID_SURFACE_FACTORY,
	SID_SURFACE_POLICE,
	SID_TUBE,
	SID_UNDERGROUND_FACTORY,
	SID_UNDERGROUND_POLICE,
	SID_UNIVERSITY,
	SID_WAREHOUSE,
	SID_RECYCLING,
	SID_MAINTENANCE_FACILITY,

	SID_COUNT
};


/**
 * Factory Product enumeration
 *
 * \note	Products are arranged to match the order in which they appear
 *			in the icon atlas (data/ui/factory.png). In order to allow
 *			for easy future additions, the icons are grouped into two
 *			sets of 32 icons. The first 32 are for above ground products,
 *			the second set for underground products.
 *
 *			To easily map to icons in the atlas, padding entries with a
 *			'reserved' naming convention have been added. These can be
 *			replaced as additional products are added.
 *
 * \remark	ASSUMPTION: Factories will never have more than 32 individual
 *			products that they can produce.
 */
enum ProductType
{
	PRODUCT_NONE = -1,

	// =====================================
	// = SURFACE FACTORIES
	// =====================================
	PRODUCT_DIGGER,
	PRODUCT_DOZER,
	PRODUCT_MINER,
	PRODUCT_EXPLORER,
	PRODUCT_TRUCK,

	PRODUCT_RESERVED_AG_05,
	PRODUCT_RESERVED_AG_06,
	PRODUCT_RESERVED_AG_07,

	PRODUCT_RESERVED_AG_08,
	PRODUCT_MAINTENANCE_PARTS,

	PRODUCT_RESERVED_AG_10,
	PRODUCT_RESERVED_AG_11,
	PRODUCT_RESERVED_AG_12,
	PRODUCT_RESERVED_AG_13,
	PRODUCT_RESERVED_AG_14,
	PRODUCT_RESERVED_AG_15,

	PRODUCT_RESERVED_AG_16,
	PRODUCT_RESERVED_AG_17,
	PRODUCT_RESERVED_AG_18,
	PRODUCT_RESERVED_AG_19,
	PRODUCT_RESERVED_AG_20,
	PRODUCT_RESERVED_AG_21,
	PRODUCT_RESERVED_AG_22,
	PRODUCT_RESERVED_AG_23,

	PRODUCT_RESERVED_AG_24,
	PRODUCT_RESERVED_AG_25,
	PRODUCT_RESERVED_AG_26,
	PRODUCT_RESERVED_AG_27,
	PRODUCT_RESERVED_AG_28,
	PRODUCT_RESERVED_AG_29,
	PRODUCT_RESERVED_AG_30,
	PRODUCT_RESERVED_AG_31,


	// =====================================
	// = UNDERGROUND FACTORIES
	// =====================================
	PRODUCT_CLOTHING,
	PRODUCT_MEDICINE,
	PRODUCT_RESERVED_UG_34,
	PRODUCT_RESERVED_UG_35,
	PRODUCT_RESERVED_UG_36,
	PRODUCT_RESERVED_UG_37,
	PRODUCT_RESERVED_UG_38,
	PRODUCT_RESERVED_UG_39,

	PRODUCT_RESERVED_UG_40,
	PRODUCT_RESERVED_UG_41,
	PRODUCT_RESERVED_UG_42,
	PRODUCT_RESERVED_UG_43,
	PRODUCT_RESERVED_UG_44,
	PRODUCT_RESERVED_UG_45,
	PRODUCT_RESERVED_UG_46,
	PRODUCT_RESERVED_UG_47,

	PRODUCT_RESERVED_UG_48,
	PRODUCT_RESERVED_UG_49,
	PRODUCT_RESERVED_UG_50,
	PRODUCT_RESERVED_UG_51,
	PRODUCT_RESERVED_UG_52,
	PRODUCT_RESERVED_UG_53,
	PRODUCT_RESERVED_UG_54,
	PRODUCT_RESERVED_UG_55,

	PRODUCT_RESERVED_UG_56,
	PRODUCT_RESERVED_UG_57,
	PRODUCT_RESERVED_UG_58,
	PRODUCT_RESERVED_UG_59,
	PRODUCT_RESERVED_UG_60,
	PRODUCT_RESERVED_UG_61,
	PRODUCT_RESERVED_UG_62,
	PRODUCT_RESERVED_UG_63,

	PRODUCT_COUNT
};


bool productTypeInRange(ProductType productType);


enum class MoraleIndexs
{
	Terrible,
	Poor,
	Fair,
	Good,
	Excellent,

	Description,
	Births,
	Deaths,
	NoFoodProduction,
	Parks,
	Recreation,
	Commercial,
	ResidentialOverflow,
	BiowasteOverflow,
	StructuresDisabled,
	StructuresDestroyed
};


extern const std::map<TerrainType, std::string> TILE_INDEX_TRANSLATION;
extern const std::map<MineProductionRate, std::string> MINE_YIELD_TRANSLATION;

extern const std::array<std::string, 4> ResourceNamesRefined;
extern const std::array<std::string, 4> ResourceNamesOre;

extern const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsRefined;
extern const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsOre;

extern const std::map<std::array<bool, 4>, std::string> IntersectionPatternTable;

void checkSavegameVersion(const std::string& filename);
NAS2D::Xml::XmlDocument openSavegame(const std::string& filename);

void setMeanSolarDistance(float newMeanSolarDistance);
float getMeanSolarDistance();

/**
 * Translates a DisabledReason enumeration into a string
 */
const std::string& disabledReason(DisabledReason);

/**
 * Translates an IdleReason enumeration into a string
 */
const std::string& idleReason(IdleReason);

/**
 * Gets a morale string based on value;
 */
const std::string& moraleString(std::size_t index);
const std::string& moraleString(MoraleIndexs morale);
std::size_t moraleStringTableCount();

void drawProgressBar(int value, int max, NAS2D::Rectangle<int> rect, int padding = 4);

NAS2D::Color structureColorFromIndex(StructureState structureState);
NAS2D::Color structureTextColorFromIndex(StructureState structureState);

int getTruckAvailability();

/**
 * \return 1 on success, 0 otherwise.
 */
int pullTruckFromInventory();

/**
 * \return 1 on success, 0 otherwise.
 */
int pushTruckIntoInventory();


const auto formatDiff = [](int diff)
{
	return ((diff > 0) ? "+" : "") + std::to_string(diff);
};
