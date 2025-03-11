#include "OreDepositYieldString.h"

#include "Constants/Strings.h"

#include <libOPHD/EnumOreDepositYield.h>

#include <map>


namespace
{
	const std::map<OreDepositYield, std::string> oreDepositYieldStringTable =
	{
		{OreDepositYield::High, constants::MineYieldHigh},
		{OreDepositYield::Low, constants::MineYieldLow},
		{OreDepositYield::Medium, constants::MineYieldMedium},
	};
}


const std::string& oreDepositYieldEnumToString(OreDepositYield oreDepositYield)
{
	return oreDepositYieldStringTable.at(oreDepositYield);
}
