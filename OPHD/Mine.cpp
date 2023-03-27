#include "Mine.h"

#include "StorableResources.h"

#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>

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
		{MineProductionRate::Low, {800, 800, 800, 800}},
		{MineProductionRate::Medium, {1000, 1000, 1000, 1000}},
		{MineProductionRate::High, {1250, 1250, 1250, 1250}}
	};


	const std::array<std::string, 4> ResourceFieldNames =
	{
		"common_metals", "common_minerals", "rare_metals", "rare_minerals"
	};


	// [Exhausted, Active, 4x miningEnabled]
	const std::bitset<6> DefaultFlags{"001111"};


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
}


Mine::Mine() :
	mFlags{DefaultFlags}
{
}


Mine::Mine(MineProductionRate rate) :
	mProductionRate{rate},
	mFlags{DefaultFlags}
{
}


bool Mine::active() const
{
	return mFlags[4];
}


void Mine::active(bool newActive)
{
	mFlags[4] = newActive;
}


std::bitset<4> Mine::miningEnabled() const
{
	// We only want ore mining enabled bits
	return {mFlags.to_ulong() & 0xF};
}


void Mine::miningEnabled(OreType oreType, bool value)
{
	mFlags[static_cast<int>(oreType)] = value;
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
 * Pulls the specified quantities of Ore from the Mine. If
 * insufficient ore is available, only pulls what's available.
 */
StorableResources Mine::pull(const StorableResources& maxTransfer)
{
	StorableResources totalTransferAmount{};
	for (auto& vein : mVeins)
	{
		const auto transferAmount = vein.cap(maxTransfer - totalTransferAmount);
		totalTransferAmount += transferAmount;
		vein -= transferAmount;
	}
	return totalTransferAmount;
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
