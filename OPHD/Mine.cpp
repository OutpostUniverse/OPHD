#include "Mine.h"

#include "StorableResources.h"

#include <NAS2D/ParserHelper.h>

#include <algorithm>
#include <array>
#include <map>


namespace
{
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
	const std::map<MineProductionRate, Mine::MineVein> YieldTable =
	{
		{MineProductionRate::Low, {600, 500, 600, 500}},
		{MineProductionRate::Medium, {700, 550, 700, 550}},
		{MineProductionRate::High, {850, 600, 850, 600}}
	};


	const std::array<std::string, 4> ResourceFieldNames =
	{
		"common_metals", "common_minerals", "rare_metals", "rare_minerals"
	};


	/**
	 * Helper function that gets the total amount of ore
	 */
	StorableResources getOreCount(const Mine::MineVeins& veins, int depth)
	{
		StorableResources availableResources{};
		for (std::size_t i = 0; i < static_cast<std::size_t>(depth); ++i)
		{
			availableResources += veins[i];
		}
		return availableResources;
	}


	void setDefaultFlags(std::bitset<6>& flags)
	{
		flags[Mine::OreType::ORE_COMMON_METALS] = true;
		flags[Mine::OreType::ORE_COMMON_MINERALS] = true;
		flags[Mine::OreType::ORE_RARE_METALS] = true;
		flags[Mine::OreType::ORE_RARE_MINERALS] = true;
		flags[4] = false;
		flags[5] = false;
	}
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


StorableResources Mine::availableResources() const
{
	return getOreCount(mVeins, depth());
}


StorableResources Mine::totalYield() const
{
	return YieldTable.at(productionRate()) * depth();
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

	int oreCount = 0;
	for (auto vein : mVeins)
	{
		oreCount += vein.total();
	}

	mFlags[5] = (oreCount == 0);
}


/**
 * Pulls the specified quantity of Ore from the Mine. If
 * insufficient ore is available, only pulls what's available.
 */
int Mine::pull(OreType type, int quantity)
{
	int pullCount = 0;

	for (auto& vein : mVeins)
	{
		const auto transferAmount = std::min(vein.resources[type], quantity - pullCount);
		pullCount += transferAmount;
		vein.resources[type] -= transferAmount;

		if (pullCount == quantity) { break; }
	}

	return pullCount;
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

	for (const auto& mineVein : mVeins)
	{
		element->linkEndChild(NAS2D::dictionaryToAttributes(
			"vein",
			{{
				{ResourceFieldNames[0], mineVein.resources[0]},
				{ResourceFieldNames[1], mineVein.resources[1]},
				{ResourceFieldNames[2], mineVein.resources[2]},
				{ResourceFieldNames[3], mineVein.resources[3]},
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

	mVeins.resize(0);
	mVeins.reserve(static_cast<std::size_t>(depth));
	for (auto* vein = element->firstChildElement(); vein != nullptr; vein = vein->nextSiblingElement())
	{
		const auto veinDictionary = NAS2D::attributesToDictionary(*vein);
		mVeins.push_back({
			veinDictionary.get<int>(ResourceFieldNames[0], 0),
			veinDictionary.get<int>(ResourceFieldNames[1], 0),
			veinDictionary.get<int>(ResourceFieldNames[2], 0),
			veinDictionary.get<int>(ResourceFieldNames[3], 0),
		});
	}
}
