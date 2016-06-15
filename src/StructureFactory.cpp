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
		case SID_SEED_FACTORY:
			_st = new SeedFactory();
			break;
		case SID_SEED_POWER:
			_st = new SeedPower();
			break;
		case SID_SEED_SMELTER:
			_st = new SeedSmelter();
			break;
		default:
			cout << "StructureFactory::get(): Unsupported structure type called." << endl;
			break;
	}

	return _st;
}