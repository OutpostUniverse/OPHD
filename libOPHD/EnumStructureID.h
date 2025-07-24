#pragma once


/**
 * Unique identifier code for each structure.
 *
 * \note	Each individual structure is identified using a SID_ code as opposed
 *			the structure Class code which is used to group like structures into
 *			lists for structure updates.
 */
enum class StructureID
{
	None,

	Agridome,
	AirShaft,
	CargoLander,
	Chap,
	ColonistLander,
	CommandCenter,
	Commercial,
	CommTower,
	FusionReactor,
	HotLaboratory,
	Laboratory,
	MedicalCenter,
	MineFacility,
	MineShaft,
	Nursery,
	Park,
	RecreationCenter,
	RedLightDistrict,
	Residence,
	Road,
	RobotCommand,
	SeedFactory,
	SeedLander,
	SeedPower,
	SeedSmelter,
	Smelter,
	SolarPanel1,
	SolarPlant,
	StorageTanks,
	SurfaceFactory,
	SurfacePolice,
	Tube,
	UndergroundFactory,
	UndergroundPolice,
	University,
	Warehouse,
	Recycling,
	MaintenanceFacility,

	Count
};
