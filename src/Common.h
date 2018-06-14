#pragma once

#include <array>
#include <memory>
#include <string>


#include <NAS2D/NAS2D.h>


/**
 * Digger robot digging direction.
 */
enum Direction
{
	DIR_UP,
	DIR_DOWN,
	DIR_EAST,
	DIR_WEST,
	DIR_NORTH,
	DIR_SOUTH
};


/**
 * Terrain type enumeration
 */
enum TerrainType
{
	TERRAIN_DOZED,
	TERRAIN_CLEAR,
	TERRAIN_ROUGH,
	TERRAIN_DIFFICULT,
	TERRAIN_IMPASSABLE
};


/**
 * Connector Direction.
 * 
 * \note	CONNECTOR_INTERSECTION is explicitely set to '1' to prevent
 *			breaking changes with save files.
 */
enum ConnectorDir
{
	CONNECTOR_INTERSECTION = 1,
	CONNECTOR_RIGHT,
	CONNECTOR_LEFT,
	CONNECTOR_VERTICAL			// Functions as an intersection
};


/**
 * Unique identifier code for each structure.
 * 
 * \note	Each individual structure is identified using a SID_ code as opposed
 *			the structure CLASS_ code which is used to group like structures into
 *			lists for structure updates.
 * 
 * \remark	
 */
enum StructureID
{
	SID_NONE,

	SID_AGRIDOME,
	SID_AIR_SHAFT,
	SID_CHAP,
	SID_COLONIST_LANDER,
	SID_COMMAND_CENTER,
	SID_COMMERCIAL,
	SID_FUSION_REACTOR,
	SID_HOT_LABORATORY,
	SID_LABORATORY,
	SID_MEDICAL_CENTER,
	SID_MINE_FACILITY,
	SID_MINE_SHAFT,
	SID_PARK,
	SID_SURFACE_POLICE,
	SID_UNDERGROUND_POLICE,
	SID_RECREATION_CENTER,
	SID_RED_LIGHT_DISTRICT,
	SID_RESIDENCE,
	SID_ROBOT_COMMAND,
	SID_SEED_FACTORY,
	SID_SEED_LANDER,
	SID_SEED_POWER,
	SID_SEED_SMELTER,
	SID_SMELTER,
	SID_SOLAR_PLANT,
	SID_STORAGE_TANKS,
	SID_SURFACE_FACTORY,
	SID_TUBE,
	SID_UNDERGROUND_FACTORY,
	SID_UNIVERSITY,
	SID_WAREHOUSE,
	SID_NURSERY,
	SID_COMM_TOWER,
	SID_CARGO_LANDER,
	SID_SOLAR_PANEL1,

	SID_COUNT
};


/**
 * Robot type enumeration
 */
enum RobotType
{
	ROBOT_NONE,
	ROBOT_DIGGER,
	ROBOT_DOZER,
	ROBOT_MINER
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

	PRODUCT_ROAD_MATERIALS,
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


/**
 * Contains population requirements for a given Structure.
 * 
 * Index 0: Workers.
 * Index 1: Scientists.
 * 
 * Any other index will result in exceptions being thrown.
 */
typedef std::array<int, 2> PopulationRequirements;


/**
 * Convenience function to pass a Rectangle_2df to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, const NAS2D::Rectangle_2df& rect);


/**
 * Convenience function to pass \c float's to \c isPointInRect()
 */
bool pointInRect_f(int x, int y, float rectX, float rectY, float rectW, float rectH);


void doNonFatalErrorMessage(const std::string& title, const std::string& msg);

void checkSavegameVersion(const std::string& filename);
