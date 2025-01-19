#pragma once

#include "EnumProductType.h"

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Color.h>

#include <map>
#include <string>
#include <vector>

namespace NAS2D::Xml
{
	class XmlDocument;
}

enum class MineProductionRate;
enum class MoraleIndexs;
enum class StructureState;
enum class TerrainType;


extern const std::map<TerrainType, std::string> TILE_INDEX_TRANSLATION;
extern const std::map<MineProductionRate, std::string> MINE_YIELD_TRANSLATION;

void checkSavegameVersion(const std::string& filename);
NAS2D::Xml::XmlDocument openSavegame(const std::string& filename);

void setMeanSolarDistance(float newMeanSolarDistance);
float getMeanSolarDistance();

const std::string& moraleString(std::size_t index);
const std::string& moraleString(MoraleIndexs morale);
std::size_t moraleStringTableCount();

NAS2D::Color structureColorFromIndex(StructureState structureState);
NAS2D::Color structureTextColorFromIndex(StructureState structureState);


const auto formatDiff = [](int diff)
{
	return ((diff > 0) ? "+" : "") + std::to_string(diff);
};
