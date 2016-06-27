#include "StructureFactory.h"

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
		case SID_RESIDENCE:
			_st = new Residence();
			break;
		case SID_SEED_FACTORY:
			_st = new SeedFactory();
			break;
		case SID_SEED_POWER:
			_st = new SeedPower();
			break;
		case SID_SEED_SMELTER:
			_st = new SeedSmelter();
			break;
		case SID_STORAGE_TANKS:
			_st = new StorageTanks();
			break;
		default:
			cout << "StructureFactory::get(): Unsupported structure type called." << endl;
			break;
	}

	return _st;
}


ResourcePool StructureFactory::costToBuild(StructureID type)
{
	/*
	 * FIXME: Horribly inefficient.
	 * 
	 * Better way to do this would be to define the structure costs in a lookup table.
	 * Question being that currently this is implemented by having the derived Structure
	 * class defining the resource cost to build.
	 * 
	 * Two possibilities -- either build a table by instantiating, copying and destroying
	 * derived object types, or move definitions from the derived class and move them to
	 * the StructureFactory class while building the table.
	 */

	/*
	 * FIXME: Prone to break if we request a cost to build on a structure that isn't,
	 * defined. We end up with a null pointer and explode the second we try to get
	 * resources.
	 */
	Structure* _st = StructureFactory::get(type);
	ResourcePool rp = _st->resourcesCostToBuild();
	delete _st;
	return rp;
}