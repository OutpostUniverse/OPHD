#include "StructureColor.h"

#include "../MapObjects/StructureState.h"

#include <map>


namespace
{
	const std::map<StructureState, NAS2D::Color> structureColorTable
	{
		{StructureState::UnderConstruction, NAS2D::Color{150, 150, 150, 100}},
		{StructureState::Operational, NAS2D::Color{0, 185, 0}},
		{StructureState::Idle, NAS2D::Color{0, 185, 0, 100}},
		{StructureState::Disabled, NAS2D::Color{220, 0, 0}},
		{StructureState::Destroyed, NAS2D::Color{220, 0, 0}}
	};


	const std::map<StructureState, NAS2D::Color> structureTextColorTable
	{
		{StructureState::UnderConstruction, NAS2D::Color{185, 185, 185, 100}},
		{StructureState::Operational, NAS2D::Color{0, 185, 0}},
		{StructureState::Idle, NAS2D::Color{0, 185, 0, 100}},
		{StructureState::Disabled, NAS2D::Color{220, 0, 0}},
		{StructureState::Destroyed, NAS2D::Color{220, 0, 0}}
	};
}


NAS2D::Color structureColorFromIndex(StructureState structureState)
{
	return structureColorTable.at(structureState);
}


NAS2D::Color structureTextColorFromIndex(StructureState structureState)
{
	return structureTextColorTable.at(structureState);
}
