#include "OreDeposit.h"

#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>

#include <algorithm>
#include <array>
#include <map>
#include <bitset>


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
	const std::map<OreDepositYield, StorableResources> YieldTable =
	{
		{OreDepositYield::Low, {800, 800, 800, 800}},
		{OreDepositYield::Medium, {1000, 1000, 1000, 1000}},
		{OreDepositYield::High, {1250, 1250, 1250, 1250}}
	};


	const std::array<std::string, 4> ResourceFieldNames =
	{
		"common_metals", "common_minerals", "rare_metals", "rare_minerals"
	};
}


OreDeposit::OreDeposit()
{
}


OreDeposit::OreDeposit(OreDepositYield yield) :
	mOreDepositYield{yield}
{
}


/**
 * Increases the depth of the mine.
 *
 * \note	This function only modifies the Mine. It has no knowledge of the
 *			maximum digging depth of a planet and doesn't modify any tiles.
 */
void OreDeposit::increaseDepth()
{
	mCurrentDepth++;
	mTappedReserves += YieldTable.at(yield());
}


/**
 * Gets the current depth of the mine.
 */
int OreDeposit::depth() const
{
	return mCurrentDepth;
}


StorableResources OreDeposit::availableResources() const
{
	return mTappedReserves;
}


StorableResources OreDeposit::totalYield() const
{
	return YieldTable.at(yield()) * depth();
}


/**
 * Indicates that there are no resources available at this mine.
 */
bool OreDeposit::exhausted() const
{
	return mTappedReserves.isEmpty();
}


/**
 * Pulls the specified quantities of Ore from the Mine. If
 * insufficient ore is available, only pulls what's available.
 */
StorableResources OreDeposit::pull(const StorableResources& maxTransfer)
{
	const auto transferAmount = mTappedReserves.cap(maxTransfer);
	mTappedReserves -= transferAmount;

	return transferAmount;
}


// ===============================================================================


/**
 * Serializes current mine information.
 */
NAS2D::Xml::XmlElement* OreDeposit::serialize(NAS2D::Point<int> location)
{
	auto saveFlags = std::bitset<5>{0b11111};
	auto* element = NAS2D::dictionaryToAttributes(
		"mine",
		{{
			{"x", location.x},
			{"y", location.y},
			{"depth", depth()},
			{"active", true},
			{"yield", static_cast<int>(yield())},
			{"flags", saveFlags.to_string()},
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


void OreDeposit::deserialize(NAS2D::Xml::XmlElement* element)
{
	const auto dictionary = NAS2D::attributesToDictionary(*element);

	mCurrentDepth = dictionary.get<int>("depth");
	mOreDepositYield = static_cast<OreDepositYield>(dictionary.get<int>("yield"));

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
