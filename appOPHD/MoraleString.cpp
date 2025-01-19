#include "MoraleString.h"

#include <array>


const std::array MoraleStringTable =
{
	std::string{"Terrible"},
	std::string{"Poor"},
	std::string{"Fair"},
	std::string{"Good"},
	std::string{"Excellent"},

	std::string{"Morale is "},
	std::string{"Births"},
	std::string{"Deaths"},
	std::string{"No active Food Production"},
	std::string{"Parks & Arboretums"},
	std::string{"Recreational Facilities"},
	std::string{"Luxury Availability"},
	std::string{"Residential Over Capacity"},
	std::string{"Biowaste Overflowing"},
	std::string{"Structures Disabled"},
	std::string{"Structures Destroyed"},
};


const std::string& moraleString(std::size_t index)
{
	return MoraleStringTable[index];
}


const std::string& moraleString(MoraleIndexs morale)
{
	return MoraleStringTable[static_cast<std::size_t>(morale)];
}


std::size_t moraleStringTableCount()
{
	return MoraleStringTable.size();
}
