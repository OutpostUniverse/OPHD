#include "PowerStructure.h"

#include "../../Constants/UiConstants.h"

#include "../../UI/StringTable.h"

#include <libOPHD/MapObjects/StructureType.h>


PowerStructure::PowerStructure(StructureID id, Tile& tile) :
	Structure{id, tile}
{
}
