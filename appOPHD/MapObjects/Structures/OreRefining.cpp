#include "OreRefining.h"

#include "../../Constants/Numbers.h"

#include <libOPHD/EnumIdleReason.h>

#include <array>


namespace
{
	std::array<int, 4> OreConversionDivisor{2, 2, 3, 3};
}


OreRefining::OreRefining(StructureID id, Tile& tile) :
	Structure{id, tile}
{
}


int OreRefining::oreConversionDivisor(std::size_t index) const
{
	return OreConversionDivisor.at(index);
}


StorableResources OreRefining::storageCapacities() const
{
	return {
		refinedOreStorageCapacity(),
		refinedOreStorageCapacity(),
		refinedOreStorageCapacity(),
		refinedOreStorageCapacity(),
	};
}


void OreRefining::think()
{
	if (isIdle() && storage() < storageCapacities())
	{
		enable();
	}

	if (operational())
	{
		updateProduction();
	}
}


void OreRefining::updateProduction()
{
	StorableResources converted{0};
	auto& ore = production();

	for (size_t i = 0; i < ore.resources.size(); ++i)
	{
		if (ore.resources[i] >= constants::SmeltingMinimumResourcesCount)
		{
			converted.resources[i] = constants::SmeltingMinimumResourcesCount / OreConversionDivisor[i];
			ore.resources[i] = ore.resources[i] - constants::SmeltingMinimumResourcesCount;
		}
	}

	auto& stored = storage();
	auto total = stored + converted;
	auto capped = total.cap(refinedOreStorageCapacity());
	auto overflow = total - capped;

	stored = capped;

	if (!overflow.isEmpty())
	{
		StorableResources deconvertedResources{
			overflow.resources[0] * OreConversionDivisor[0],
			overflow.resources[1] * OreConversionDivisor[1],
			overflow.resources[2] * OreConversionDivisor[2],
			overflow.resources[3] * OreConversionDivisor[3]
		};

		ore += deconvertedResources;

		if (ore >= storageCapacities())
		{
			idle(IdleReason::InternalStorageFull);
		}
	}
}
