#include "AirShaft.h"

#include "../StructureState.h"
#include "../../Constants/Strings.h"

#include <libOPHD/EnumStructureID.h>


AirShaft::AirShaft(Tile& tile) :
	Structure{StructureID::AirShaft, tile}
{
	mStructureState = StructureState::Operational;
}
