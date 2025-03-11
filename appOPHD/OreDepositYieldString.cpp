#include "OreDepositYieldString.h"

#include "Constants/Strings.h"

#include <libOPHD/EnumOreDepositYield.h>

#include <map>


namespace
{
	const std::map<OreDepositYield, std::string> oreDepositYieldStringTable =
	{
		{OreDepositYield::High, constants::OreDepositYieldHigh},
		{OreDepositYield::Low, constants::OreDepositYieldLow},
		{OreDepositYield::Medium, constants::OreDepositYieldMedium},
	};
}


const std::string& oreDepositYieldEnumToString(OreDepositYield oreDepositYield)
{
	return oreDepositYieldStringTable.at(oreDepositYield);
}
