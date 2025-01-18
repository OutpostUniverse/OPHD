#pragma once

#include "EnumProductType.h"

#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Renderer/Color.h>

#include <array>
#include <map>
#include <string>
#include <vector>

namespace NAS2D::Xml
{
	class XmlDocument;
}

enum class Difficulty;
enum class DisabledReason;
enum class IdleReason;
enum class MineProductionRate;
enum class MoraleIndexs;
enum class StructureState;
enum class TerrainType;


extern const std::map<TerrainType, std::string> TILE_INDEX_TRANSLATION;
extern const std::map<MineProductionRate, std::string> MINE_YIELD_TRANSLATION;

extern const std::array<std::string, 4> ResourceNamesRefined;
extern const std::array<std::string, 4> ResourceNamesOre;

extern const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsRefined;
extern const std::array<NAS2D::Rectangle<int>, 4> ResourceImageRectsOre;

extern const std::map<std::array<bool, 4>, std::string> IntersectionPatternTable;

void checkSavegameVersion(const std::string& filename);
NAS2D::Xml::XmlDocument openSavegame(const std::string& filename);

void setMeanSolarDistance(float newMeanSolarDistance);
float getMeanSolarDistance();

const std::string& moraleString(std::size_t index);
const std::string& moraleString(MoraleIndexs morale);
std::size_t moraleStringTableCount();

void drawProgressBar(int value, int max, NAS2D::Rectangle<int> rect, int padding = 4);

NAS2D::Color structureColorFromIndex(StructureState structureState);
NAS2D::Color structureTextColorFromIndex(StructureState structureState);

int getTruckAvailability();

/**
 * \return 1 on success, 0 otherwise.
 */
int pullTruckFromInventory();

/**
 * \return 1 on success, 0 otherwise.
 */
int pushTruckIntoInventory();


const auto formatDiff = [](int diff)
{
	return ((diff > 0) ? "+" : "") + std::to_string(diff);
};
