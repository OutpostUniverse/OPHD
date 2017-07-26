#include "StructureFactory.h"


//vector<ResourcePool> StructureFactory::mStructureCostTable;
std::array<ResourcePool, SID_COUNT> StructureFactory::mStructureCostTable;
std::array<ResourcePool, SID_COUNT> StructureFactory::mStructureRecycleValueTable;
std::array<PopulationRequirements, SID_COUNT> StructureFactory::mPopulationRequirementsTable = { 0 };


/**
 * Gets a new Structure object given a StructureID.
 * 
 * \param	type	A valid StructureID value.
 * 
 * \return	Pointer to a newly constructed Structure or
 *			\c nullptr if structure type unsupported/invalid.
 */
Structure* StructureFactory::get(StructureID type)
{
	Structure* _st = nullptr;

	// This seems like a naive approach... I usually see these implemented as the base
	// object type has a static function that is used as an interface to instantiate
	// derived types.
	switch (type)
	{
		case SID_NONE:
			break;

		case SID_AGRIDOME:
			_st = new Agridome();
			break;

		case SID_AIR_SHAFT:
			_st = new AirShaft();
			break;

		case SID_CHAP:
			_st = new CHAP();
			break;

		case SID_COLONIST_LANDER: // only here for loading games
			_st = new ColonistLander(nullptr);
			break;

		case SID_COMMAND_CENTER:
			_st = new CommandCenter();
			break;

		case SID_COMMERCIAL:
			_st = new Commercial();
			break;

		case SID_FUSION_REACTOR:
			_st = new FusionReactor();
			break;

		case SID_HOT_LABORATORY:
			_st = new HotLaboratory();
			break;

		case SID_LABORATORY:
			_st = new Laboratory();
			break;

		case SID_MEDICAL_CENTER:
			_st = new MedicalCenter();
			break;

		case SID_MINE_FACILITY: // only here for loading games
			_st = new MineFacility(nullptr);
			break;

		case SID_MINE_SHAFT: // only here for loading games
			_st = new MineShaft();
			break;

		case SID_PARK:
			_st = new Park();
			break;

		case SID_SURFACE_POLICE:
			_st = new SurfacePolice();
			break;

		case SID_UNDERGROUND_POLICE:
			_st = new UndergroundPolice();
			break;

		case SID_RECREATION_CENTER:
			_st = new RecreationCenter();
			break;

		case SID_RED_LIGHT_DISTRICT:
			_st = new RedLightDistrict();
			break;

		case SID_RESIDENCE:
			_st = new Residence();
			break;

		case SID_ROBOT_COMMAND:
			_st = new RobotCommand();
			break;

		case SID_SEED_FACTORY:
			_st = new SeedFactory();
			break;

		case SID_SEED_LANDER: // only here for loading games
			_st = new SeedLander(0, 0);
			break;

		case SID_SEED_POWER:
			_st = new SeedPower();
			break;

		case SID_SEED_SMELTER:
			_st = new SeedSmelter();
			break;

		case SID_SMELTER:
			_st = new Smelter();
			break;

		case SID_SOLAR_PLANT:
			_st = new SolarPlant();
			break;

		case SID_STORAGE_TANKS:
			_st = new StorageTanks();
			break;

		case SID_SURFACE_FACTORY:
			_st = new SurfaceFactory();
			break;

		case SID_UNDERGROUND_FACTORY:
			_st = new UndergroundFactory();
			break;

		case SID_UNIVERSITY:
			_st = new University();
			break;

		case SID_WAREHOUSE:
			_st = new Warehouse();
			break;

		default:
			cout << "StructureFactory::get(): Unsupported structure type called." << endl;
			break;
	}

	return _st;
}


/**
 * Gets the population required to operate a given Structure.
 * 
 * \param	type	A valid StructureID value.
 */
const PopulationRequirements& StructureFactory::populationRequirements(StructureID type)
{
	return mPopulationRequirementsTable[type];
}


/**
 * Gets the cost in resources required to build a given Structure.
 * 
 * \param	type	A valid StructureID value.
 */
const ResourcePool& StructureFactory::costToBuild(StructureID type)
{
	return mStructureCostTable[type];
}


/**
 * Gets the recycling value of a specified structure type.
 * 
 * \param	type	A valid StructureID value.
 */
ResourcePool& StructureFactory::recyclingValue(StructureID type)
{
	return mStructureCostTable[type];
}


/**
 * Initializes StructureFactory and builds the requirements tables.
 */
void StructureFactory::init()
{
	buildCostTable();
	buildRecycleValueTable();
	buildPopulationRequirementsTable();
}


/**
 * Indicates that the source ResourcePool has enough resources to accommodate
 * the resource requirements of the specificed structure.
 */
bool StructureFactory::canBuild(const ResourcePool& source, StructureID type)
{
	ResourcePool _rp = StructureFactory::costToBuild(type);

	if (source.commonMetals() < _rp.commonMetals() || source.commonMinerals() < _rp.commonMinerals() ||
		source.rareMetals() < _rp.rareMetals() || source.rareMinerals() < _rp.rareMinerals())
		return false;

	return true;
}


/**
 * Fills out the build costs for all structures.
 */
void StructureFactory::buildCostTable()
{
	mStructureCostTable[SID_AGRIDOME] = ResourcePool(0, 0, 0, 0, 20, 10, 5, 0, 0, 0);
	mStructureCostTable[SID_AIR_SHAFT] = ResourcePool();
	mStructureCostTable[SID_CHAP] = ResourcePool(0, 0, 0, 0, 50, 10, 20, 5, 0, 0);
	mStructureCostTable[SID_COMMAND_CENTER] = ResourcePool(0, 0, 0, 0, 100, 75, 65, 35, 0, 0);
	mStructureCostTable[SID_COMMERCIAL] = ResourcePool(0, 0, 0, 0, 25, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_FUSION_REACTOR] = ResourcePool(0, 0, 0, 0, 75, 25, 50, 30, 0, 0);
	mStructureCostTable[SID_HOT_LABORATORY] = ResourcePool(0, 0, 0, 0, 20, 10, 10, 5, 0, 0);
	mStructureCostTable[SID_LABORATORY] = ResourcePool(0, 0, 0, 0, 20, 10, 10, 5, 0, 0);
	mStructureCostTable[SID_MEDICAL_CENTER] = ResourcePool(0, 0, 0, 0, 20, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_MINE_FACILITY] = ResourcePool(0, 0, 0, 0, 20, 10, 5, 0, 0, 0);
	mStructureCostTable[SID_PARK] = ResourcePool(0, 0, 0, 0, 25, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_SURFACE_POLICE] = ResourcePool(0, 0, 0, 0, 25, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_UNDERGROUND_POLICE] = ResourcePool(0, 0, 0, 0, 25, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_RECREATION_CENTER] = ResourcePool(0, 0, 0, 0, 25, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_RED_LIGHT_DISTRICT] = ResourcePool(0, 0, 0, 0, 25, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_RESIDENCE] = ResourcePool(0, 0, 0, 0, 25, 5, 2, 0, 0, 0);
	mStructureCostTable[SID_ROBOT_COMMAND] = ResourcePool(0, 0, 0, 0, 75, 50, 45, 25, 0, 0);
	mStructureCostTable[SID_SEED_FACTORY] = ResourcePool(0, 0, 0, 0, 20, 10, 10, 5, 0, 0);
	mStructureCostTable[SID_SEED_LANDER] = ResourcePool(0, 0, 0, 0, 10, 10, 5, 5, 0, 0);
	mStructureCostTable[SID_SEED_POWER] = ResourcePool(0, 0, 0, 0, 15, 10, 10, 8, 0, 0);
	mStructureCostTable[SID_SEED_SMELTER] = ResourcePool(0, 0, 0, 0, 25, 20, 10, 5, 0, 0);
	mStructureCostTable[SID_SMELTER] = ResourcePool(0, 0, 0, 0, 30, 20, 10, 5, 0, 0);
	mStructureCostTable[SID_SOLAR_PLANT] = ResourcePool(0, 0, 0, 0, 50, 25, 50, 20, 0, 0);
	mStructureCostTable[SID_STORAGE_TANKS] = ResourcePool(0, 0, 0, 0, 15, 5, 6, 1, 0, 0);
	mStructureCostTable[SID_SURFACE_FACTORY] = ResourcePool(0, 0, 0, 0, 20, 10, 10, 5, 0, 0);
	mStructureCostTable[SID_TUBE] = ResourcePool();
	mStructureCostTable[SID_UNDERGROUND_FACTORY] = ResourcePool(0, 0, 0, 0, 20, 10, 10, 5, 0, 0);
	mStructureCostTable[SID_UNIVERSITY] = ResourcePool(0, 0, 0, 0, 20, 10, 10, 5, 0, 0);
	mStructureCostTable[SID_WAREHOUSE] = ResourcePool(0, 0, 0, 0, 15, 5, 6, 1, 0, 0);
}


/**
 * Fills out the recycle value for all structures.
 */
void StructureFactory::buildRecycleValueTable()
{
	for (size_t i = 0; i < SID_COUNT; ++i)
		mStructureRecycleValueTable[static_cast<StructureID>(i)] = recycleValue(static_cast<StructureID>(i));
}


/**
 * Fills out the population requirements for all structures.
 */
void StructureFactory::buildPopulationRequirementsTable()
{
	mPopulationRequirementsTable[SID_AGRIDOME] = { 0 };
	mPopulationRequirementsTable[SID_AIR_SHAFT] = { 0 };
	mPopulationRequirementsTable[SID_CHAP] = { 0 };
	mPopulationRequirementsTable[SID_COMMAND_CENTER] = { 0 };
	mPopulationRequirementsTable[SID_COMMERCIAL] = { 0 };
	mPopulationRequirementsTable[SID_FUSION_REACTOR] = { 0 };
	mPopulationRequirementsTable[SID_HOT_LABORATORY] = { 0 };
	mPopulationRequirementsTable[SID_LABORATORY] = { 0 };
	mPopulationRequirementsTable[SID_MEDICAL_CENTER] = { 0 };
	mPopulationRequirementsTable[SID_MINE_FACILITY] = { 0 };
	mPopulationRequirementsTable[SID_PARK] = { 0 };
	mPopulationRequirementsTable[SID_SURFACE_POLICE] = { 0 };
	mPopulationRequirementsTable[SID_UNDERGROUND_POLICE] = { 0 };
	mPopulationRequirementsTable[SID_RECREATION_CENTER] = { 0 };
	mPopulationRequirementsTable[SID_RED_LIGHT_DISTRICT] = { 0 };
	mPopulationRequirementsTable[SID_RESIDENCE] = { 0 };
	mPopulationRequirementsTable[SID_ROBOT_COMMAND] = { 0 };
	mPopulationRequirementsTable[SID_SEED_FACTORY] = { 0 };
	mPopulationRequirementsTable[SID_SEED_LANDER] = { 0 };
	mPopulationRequirementsTable[SID_SEED_POWER] = { 0 };
	mPopulationRequirementsTable[SID_SEED_SMELTER] = { 0 };
	mPopulationRequirementsTable[SID_SMELTER] = { 0 };
	mPopulationRequirementsTable[SID_SOLAR_PLANT] = { 0 };
	mPopulationRequirementsTable[SID_STORAGE_TANKS] = { 0 };
	mPopulationRequirementsTable[SID_SURFACE_FACTORY] = { 0 };
	mPopulationRequirementsTable[SID_TUBE] = { 0 };
	mPopulationRequirementsTable[SID_UNDERGROUND_FACTORY] = { 0 };
	mPopulationRequirementsTable[SID_UNIVERSITY] = { 0 };
	mPopulationRequirementsTable[SID_WAREHOUSE] = { 0 };
}


/**
 * Calculates the base recycling value of a given structure.
 * 
 * \param	type	A valid StructureID value.
 */
ResourcePool StructureFactory::recycleValue(StructureID type)
{
	if (mStructureCostTable.empty())
		throw std::runtime_error("StructureFactory::recycleValue() called before StructureFactory::buildCostTable().");

	ResourcePool _rp = mStructureCostTable[type];

	/** Truncation of value from float to int cast is intended and desired behavior. */
	return ResourcePool(static_cast<int>(_rp.commonMetalsOre() * 0.9f),
						static_cast<int>(_rp.commonMineralsOre() * 0.9f),
						static_cast<int>(_rp.rareMetalsOre() * 0.9f),
						static_cast<int>(_rp.rareMineralsOre() * 0.9f),
						static_cast<int>(_rp.commonMetals() * 0.9f),
						static_cast<int>(_rp.commonMinerals() * 0.9f),
						static_cast<int>(_rp.rareMetals() * 0.9f),
						static_cast<int>(_rp.rareMinerals() * 0.9f),
						static_cast<int>(_rp.food() * 0.9f),
						static_cast<int>(_rp.energy() * 0.9f));
}
