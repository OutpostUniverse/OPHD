#pragma once

#include "EnumProductType.h"

#include "MineProductionRateString.h"

#include <NAS2D/Math/Rectangle.h>

#include <map>
#include <string>
#include <vector>

namespace NAS2D::Xml
{
	class XmlDocument;
}

enum class TerrainType;


extern const std::map<TerrainType, std::string> TILE_INDEX_TRANSLATION;

void checkSavegameVersion(const std::string& filename);
NAS2D::Xml::XmlDocument openSavegame(const std::string& filename);

void setMeanSolarDistance(float newMeanSolarDistance);
float getMeanSolarDistance();


const auto formatDiff = [](int diff)
{
	return ((diff > 0) ? "+" : "") + std::to_string(diff);
};
