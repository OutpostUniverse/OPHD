#include "Mine.h"

#include <NAS2D/ParserHelper.h>

#include <iostream>

#include <array>
#include <map>

using namespace NAS2D::Xml;


/**
 * Yield ore table
 * 
 * \note Follows the array layout conventions of the StorableResources class
 * 
 * [0] Common Metals
 * [1] Common Minerals
 * [2] Rare Metals
 * [3] Rare Minerals
 */
static const std::map<MineProductionRate, std::array<int, 4>> YieldTable =
{
	{ MineProductionRate::Low, { 600, 500, 600, 500 }},
	{ MineProductionRate::Medium, { 700, 550, 700, 550 }},
	{ MineProductionRate::High, { 850, 600, 850, 600 }}
};


/**
 * Helper function that gets the total amount of ore 
 */
static int getOreCount(const Mine::MineVeins& veins, Mine::OreType ore, int depth)
{
	int value = 0;
	for (std::size_t i = 0; i < static_cast<std::size_t>(depth); ++i)
	{
		value += veins[i][ore];
	}
	return value;
}


static void setDefaultFlags(std::bitset<6>& flags)
{
	flags[Mine::OreType::ORE_COMMON_METALS] = true;
	flags[Mine::OreType::ORE_COMMON_MINERALS] = true;
	flags[Mine::OreType::ORE_RARE_METALS] = true;
	flags[Mine::OreType::ORE_RARE_MINERALS] = true;
	flags[4] = false;
	flags[5] = false;
}


Mine::Mine()
{
	setDefaultFlags(mFlags);
}


Mine::Mine(MineProductionRate rate) :
	mProductionRate(rate)
{
	setDefaultFlags(mFlags);
}


bool Mine::active() const
{
	return mFlags[4];
}


void Mine::active(bool newActive)
{
	mFlags[4] = newActive;
}


bool Mine::miningCommonMetals() const
{
	return mFlags[OreType::ORE_COMMON_METALS];
}


bool Mine::miningCommonMinerals() const
{
	return mFlags[OreType::ORE_COMMON_MINERALS];
}


bool Mine::miningRareMetals() const
{
	return mFlags[OreType::ORE_RARE_METALS];
}


bool Mine::miningRareMinerals() const
{
	return mFlags[OreType::ORE_RARE_MINERALS];
}


void Mine::miningCommonMetals(bool value)
{
	mFlags[OreType::ORE_COMMON_METALS] = value;
}


void Mine::miningCommonMinerals(bool value)
{
	mFlags[OreType::ORE_COMMON_MINERALS] = value;
}


void Mine::miningRareMetals(bool value)
{
	mFlags[OreType::ORE_RARE_METALS] = value;
}


void Mine::miningRareMinerals(bool value)
{
	mFlags[OreType::ORE_RARE_MINERALS] = value;
}


/**
 * Increases the depth of the mine.
 * 
 * \note	This function only modifies the Mine. It has no knowledge of the
 *			maximum digging depth of a planet and doesn't modify any tiles.
 */
void Mine::increaseDepth()
{
	mVeins.push_back(YieldTable.at(productionRate()));
}


/**
 * Gets the current depth of the mine.
 */
int Mine::depth() const
{
	return static_cast<int>(mVeins.size());
}


/**
 * Convenience function that gets the common metal ore available
 * given the current level.
 */
int Mine::commonMetalsAvailable() const
{
	return getOreCount(mVeins, OreType::ORE_COMMON_METALS, depth());
}


/**
 * Convenience function that gets the common mineral ore available
 * given the current level.
 */
int Mine::commonMineralsAvailable() const
{
	return getOreCount(mVeins, OreType::ORE_COMMON_MINERALS, depth());
}


/**
 * Convenience function that gets the rare metal ore available
 * given the current level.
 */
int Mine::rareMetalsAvailable() const
{
	return getOreCount(mVeins, OreType::ORE_RARE_METALS, depth());
}


/**
 * Convenience function that gets the rare mineral ore available
 * given the current level.
 */
int Mine::rareMineralsAvailable() const
{
	return getOreCount(mVeins, OreType::ORE_RARE_MINERALS, depth());
}


/**
 * Gets the available count of a specified resource
 *
 * Follows the array index conventions of a StorableResource
 *
 * [0] Common Metals
 * [1] Common Minerals
 * [2] Rare Metals
 * [3] Rare Minerals
 *
 * \throws std::out_of_range index is out of range
 */
int Mine::oreAvailable(size_t index) const
{
	return getOreCount(mVeins, static_cast<Mine::OreType>(index), depth());
}


/**
 * Gets the total yield of a specified resource
 * 
 * Follows the array index conventions of a StorableResource
 * 
 * [0] Common Metals
 * [1] Common Minerals
 * [2] Rare Metals
 * [3] Rare Minerals
 * 
 * \throws std::out_of_range index is out of range
 */
int Mine::oreTotalYield(size_t index) const
{
	return YieldTable.at(productionRate())[index] * depth();
}


/**
 * Indicates that there are no resources available at this mine.
 */
bool Mine::exhausted() const
{
	return mFlags[5];
}


/**
 * Checks if the mine is exhausted and if it is sets the exhausted flag.
 * 
 * \note	This function is provided so that this computation is only
 *			done once per turn instead of being done every frame. The
 *			issue came up after updating the minimap code to indicate
 *			exhausted mines.
 */
void Mine::checkExhausted()
{
	if (!active()) { return; }

	int ore_count = 0;
	for (auto vein : mVeins)
	{
		ore_count += vein[OreType::ORE_COMMON_METALS];
		ore_count += vein[OreType::ORE_COMMON_MINERALS];
		ore_count += vein[OreType::ORE_RARE_METALS];
		ore_count += vein[OreType::ORE_RARE_MINERALS];
	}

	mFlags[5] = (ore_count == 0);
}


/**
 * Pulls the specified quantity of Ore from the Mine. If
 * insufficient ore is available, only pulls what's available.
 */
int Mine::pull(OreType type, int quantity)
{
	int pulled_count = 0, to_pull = quantity;

	for (std::size_t i = 0; i < mVeins.size(); ++i)
	{
		MineVein& vein = mVeins[i];

		if (vein[type] >= to_pull)
		{
			pulled_count = to_pull;
			vein[type] -= to_pull;
			break;
		}
		else if (vein[type] < to_pull)
		{
			pulled_count += vein[type];
			to_pull = to_pull - vein[type];
			vein[type] = 0;
		}
	}

	return pulled_count;
}


// ===============================================================================


/**
 * Serializes current mine information.
 */
NAS2D::Xml::XmlElement* Mine::serialize(NAS2D::Point<int> location)
{
	auto* element = NAS2D::dictionaryToAttributes(
		"mine",
		{{
			{"x", location.x},
			{"y", location.y},
			{"depth", depth()},
			{"active", active()},
			{"yield", static_cast<int>(productionRate())},
			{"flags", mFlags.to_string()},
		}}
	);

	for (std::size_t i = 0; i < mVeins.size(); ++i)
	{
		const MineVein& mineVein = mVeins[i];

		element->linkEndChild(NAS2D::dictionaryToAttributes(
			"vein",
			{{
				{"id", static_cast<int>(i)},
				{"common_metals", mineVein[OreType::ORE_COMMON_METALS]},
				{"common_minerals", mineVein[OreType::ORE_COMMON_MINERALS]},
				{"rare_metals", mineVein[OreType::ORE_RARE_METALS]},
				{"rare_minerals", mineVein[OreType::ORE_RARE_MINERALS]},
			}}
		));
	}

	return element;
}


void Mine::deserialize(NAS2D::Xml::XmlElement* element)
{
	const auto dictionary = NAS2D::attributesToDictionary(*element);

	const auto active = dictionary.get<bool>("active");
	const auto depth = dictionary.get<int>("depth");
	const auto yield = dictionary.get<int>("yield");
	mFlags = std::bitset<6>(dictionary.get("flags"));

	this->active(active);
	mProductionRate = static_cast<MineProductionRate>(yield);

	mVeins.resize(static_cast<std::size_t>(depth));
	for (auto* vein = element->firstChildElement(); vein != nullptr; vein = vein->nextSiblingElement())
	{
		auto mineVein = MineVein{0, 0, 0, 0};
		auto* attribute = vein->firstAttribute();
		int id = 0;
		while (attribute)
		{
			if (attribute->name() == "common_metals") { attribute->queryIntValue(mineVein[OreType::ORE_COMMON_METALS]); }
			else if (attribute->name() == "common_minerals") { attribute->queryIntValue(mineVein[OreType::ORE_COMMON_MINERALS]); }
			else if (attribute->name() == "rare_metals") { attribute->queryIntValue(mineVein[OreType::ORE_RARE_METALS]); }
			else if (attribute->name() == "rare_minerals") { attribute->queryIntValue(mineVein[OreType::ORE_RARE_MINERALS]); }
			else if (attribute->name() == "id") { attribute->queryIntValue(id); }
			attribute = attribute->next();
		}
		mVeins[static_cast<std::size_t>(id)] = mineVein;
	}
}
