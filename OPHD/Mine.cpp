#include "Mine.h"

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
	const std::map<MineProductionRate, StorableResources> YieldTable =
	{
		{MineProductionRate::Low, {800, 800, 800, 800}},
		{MineProductionRate::Medium, {1000, 1000, 1000, 1000}},
		{MineProductionRate::High, {1250, 1250, 1250, 1250}}
	};


	const std::array<std::string, 4> ResourceFieldNames =
	{
		"common_metals", "common_minerals", "rare_metals", "rare_minerals"
	};


	// [Active, 4x miningEnabled]
	const std::bitset<5> DefaultFlags{"01111"};
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
	mFlags[static_cast<std::size_t>(oreType)] = value;
}


/**
 * Increases the depth of the mine.
 * 
 * \note	This function only modifies the Mine. It has no knowledge of the
 *			maximum digging depth of a planet and doesn't modify any tiles.
 */
void Mine::increaseDepth()
{
	mCurrentDepth++;
	mTappedReserves += YieldTable.at(productionRate());
}


/**
 * Gets the current depth of the mine.
 */
int Mine::depth() const
{
	return mCurrentDepth;
}


StorableResources Mine::availableResources() const
{
	return mTappedReserves;
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
	return mTappedReserves.isEmpty();
}


/**
 * Pulls the specified quantities of Ore from the Mine. If
 * insufficient ore is available, only pulls what's available.
 */
StorableResources Mine::pull(const StorableResources& maxTransfer)
{
	const auto transferAmount = mTappedReserves.cap(maxTransfer);
	mTappedReserves -= transferAmount;

	return transferAmount;
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

	if (!mTappedReserves.isEmpty())
	{
		element->linkEndChild(NAS2D::dictionaryToAttributes(
			"vein",
			{{
				{ResourceFieldNames[0], mTappedReserves.resources[0]},
				{ResourceFieldNames[1], mTappedReserves.resources[1]},
				{ResourceFieldNames[2], mTappedReserves.resources[2]},
				{ResourceFieldNames[3], mTappedReserves.resources[3]},
			}}
		));
	}

	return element;
}


void Mine::deserialize(NAS2D::Xml::XmlElement* element)
{
	const auto dictionary = NAS2D::attributesToDictionary(*element);

	mCurrentDepth = dictionary.get<int>("depth");
	mProductionRate = static_cast<MineProductionRate>(dictionary.get<int>("yield"));
	const auto active = dictionary.get<bool>("active");
	mFlags = std::bitset<5>(dictionary.get("flags"));

	this->active(active);

	mTappedReserves = {};
	// Keep the vein iteration so we can still load old saved games
	for (auto* vein = element->firstChildElement(); vein != nullptr; vein = vein->nextSiblingElement())
	{
		const auto veinDictionary = NAS2D::attributesToDictionary(*vein);
		const auto veinReserves = StorableResources{
			veinDictionary.get<int>(ResourceFieldNames[0], 0),
			veinDictionary.get<int>(ResourceFieldNames[1], 0),
			veinDictionary.get<int>(ResourceFieldNames[2], 0),
			veinDictionary.get<int>(ResourceFieldNames[3], 0),
		};
		mTappedReserves += veinReserves;
	}
}
