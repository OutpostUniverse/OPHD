#pragma once


/**
	* Class of a Structure.
	*
	* Structures are grouped by 'class'. Basically it's just an easy
	* way to know how to sort structures so that they can be updated
	* based on priority.
	*
	* \note	Some structure classes will only have one structure
	*			that uses it. This is intended behavior.
	*/
enum class StructureClass
{
	Command,
	Communication,
	Commercial,
	EnergyProduction,
	Factory,
	FoodProduction,
	Laboratory,
	Lander,
	LifeSupport,
	Maintenance,
	Mine,
	MedicalCenter,
	Nursery,
	Park,
	Road,
	SurfacePolice,
	UndergroundPolice,
	RecreationCenter,
	Recycling,
	Residence,
	RobotCommand,
	Smelter,
	Storage,
	Tube,
	Undefined, /**< Used for structures that have no need for classification. */
	University,
	Warehouse
};
