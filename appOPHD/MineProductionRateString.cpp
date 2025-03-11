#include "MineProductionRateString.h"

#include "Constants/Strings.h"

#include <libOPHD/EnumOreDepositYield.h>

#include <map>


namespace
{
	const std::map<MineProductionRate, std::string> mineProductionRateStringTable =
	{
		{MineProductionRate::High, constants::MineYieldHigh},
		{MineProductionRate::Low, constants::MineYieldLow},
		{MineProductionRate::Medium, constants::MineYieldMedium},
	};
}


const std::string& mineProductionRateEnumToString(MineProductionRate mineProductionRate)
{
	return mineProductionRateStringTable.at(mineProductionRate);
}
