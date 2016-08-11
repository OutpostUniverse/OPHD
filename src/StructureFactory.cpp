#include "StructureFactory.h"

StructureFactory::StructureCostTable StructureFactory::mStructureCostTable;

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


const ResourcePool& StructureFactory::costToBuild(StructureID type)
{
	if (mStructureCostTable.empty())
		buildCostTable();

	return mStructureCostTable[type];
}


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


ResourcePool StructureFactory::recyclingValue(StructureID type)
{
	if (mStructureCostTable.empty())
		buildCostTable();

	return ResourcePool(	mStructureCostTable[type].commonMetalsOre() * 0.9f,
							mStructureCostTable[type].commonMineralsOre() * 0.9f,
							mStructureCostTable[type].rareMetalsOre() * 0.9f,
							mStructureCostTable[type].rareMineralsOre() * 0.9f,
							mStructureCostTable[type].commonMetals() * 0.9f,
							mStructureCostTable[type].commonMinerals() * 0.9f,
							mStructureCostTable[type].rareMetals() * 0.9f,
							mStructureCostTable[type].rareMinerals() * 0.9f,
							mStructureCostTable[type].food() * 0.9f,
							mStructureCostTable[type].energy() * 0.9f);
}