#include "StructureColor.h"

#include "../MapObjects/Structure.h"

#include <map>

using namespace NAS2D;


const std::map<StructureState, Color> STRUCTURE_COLOR_TABLE
{
	{StructureState::UnderConstruction, Color{150, 150, 150, 100}},
	{StructureState::Operational, Color{0, 185, 0}},
	{StructureState::Idle, Color{0, 185, 0, 100}},
	{StructureState::Disabled, Color{220, 0, 0}},
	{StructureState::Destroyed, Color{220, 0, 0}}
};


const std::map<StructureState, Color> STRUCTURE_TEXT_COLOR_TABLE
{
	{StructureState::UnderConstruction, Color{185, 185, 185, 100}},
	{StructureState::Operational, Color{0, 185, 0}},
	{StructureState::Idle, Color{0, 185, 0, 100}},
	{StructureState::Disabled, Color{220, 0, 0}},
	{StructureState::Destroyed, Color{220, 0, 0}}
};


Color structureColorFromIndex(StructureState structureState)
{
	return STRUCTURE_COLOR_TABLE.at(structureState);
}


Color structureTextColorFromIndex(StructureState structureState)
{
	return STRUCTURE_TEXT_COLOR_TABLE.at(structureState);
}
