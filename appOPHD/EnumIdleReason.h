#pragma once


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
