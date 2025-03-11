#include "OreDepositYieldString.h"

#include "Constants/Strings.h"

#include <libOPHD/EnumOreDepositYield.h>

#include <map>


namespace
{
	const std::map<OreDepositYield, std::string> mineProductionRateStringTable =
	{
		{OreDepositYield::High, constants::MineYieldHigh},
		{OreDepositYield::Low, constants::MineYieldLow},
		{OreDepositYield::Medium, constants::MineYieldMedium},
	};
}


const std::string& mineProductionRateEnumToString(OreDepositYield mineProductionRate)
{
	return mineProductionRateStringTable.at(mineProductionRate);
}
