// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Mine.h"

#include <iostream>

using namespace NAS2D::Xml;


/**
 * Helper function that gets the total amount of ore 
 */
static int getOreCount(const Mine::MineVeins& veins, Mine::OreType ore, int depth)
{
	int _value = 0;
	for (std::size_t i = 0; i < static_cast<std::size_t>(depth); ++i)
	{
		_value += veins[i][ore];
	}
	return _value;
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


/**
 * C'tor
 */
Mine::Mine()
{
	setDefaultFlags(mFlags);
}


/**
 * C'tor
 */
Mine::Mine(MineProductionRate rate) : mProductionRate(rate)
{
	setDefaultFlags(mFlags);
}


/**
 * 
 */
bool Mine::active() const
{
	return mFlags[4];
}


/**
 * 
 */
void Mine::active(bool newActive)
{
	mFlags[4] = newActive;
}


/**
 * 
 */
bool Mine::miningCommonMetals() const
{
	return mFlags[OreType::ORE_COMMON_METALS];
}


/**
 * 
 */
bool Mine::miningCommonMinerals() const
{
	return mFlags[OreType::ORE_COMMON_MINERALS];
}


/**
 * 
 */
bool Mine::miningRareMetals() const
{
	return mFlags[OreType::ORE_RARE_METALS];
}


/**
 * 
 */
bool Mine::miningRareMinerals() const
{
	return mFlags[OreType::ORE_RARE_MINERALS];
}


/**
 * 
 */
void Mine::miningCommonMetals(bool value)
{
	mFlags[OreType::ORE_COMMON_METALS] = value;
}


/**
 * 
 */
void Mine::miningCommonMinerals(bool value)
{
	mFlags[OreType::ORE_COMMON_MINERALS] = value;
}


/**
 * 
 */
void Mine::miningRareMetals(bool value)
{
	mFlags[OreType::ORE_RARE_METALS] = value;
}


/**
 * 
 */
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
	auto vein = MineVein{0,0,0,0};

	switch (productionRate())
	{
	case MineProductionRate::Low:
		vein[OreType::ORE_COMMON_METALS] = 600;
		vein[OreType::ORE_COMMON_MINERALS] = 500;
		vein[OreType::ORE_RARE_METALS] = 600;
		vein[OreType::ORE_RARE_MINERALS] = 500;
		break;

	case MineProductionRate::Medium:
		vein[OreType::ORE_COMMON_METALS] = 700;
		vein[OreType::ORE_COMMON_MINERALS] = 550;
		vein[OreType::ORE_RARE_METALS] = 700;
		vein[OreType::ORE_RARE_MINERALS] = 550;
		break;

	case MineProductionRate::High:
		vein[OreType::ORE_COMMON_METALS] = 850;
		vein[OreType::ORE_COMMON_MINERALS] = 600;
		vein[OreType::ORE_RARE_METALS] = 850;
		vein[OreType::ORE_RARE_MINERALS] = 600;
		break;
	}

	mVeins.push_back(vein);
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
void Mine::serialize(NAS2D::Xml::XmlElement* element)
{
	element->attribute("depth", depth());
	element->attribute("active", active());
	element->attribute("yield", static_cast<int>(productionRate()));
	element->attribute("flags", mFlags.to_string());

	for (std::size_t i = 0; i < mVeins.size(); ++i)
	{
		const MineVein& mv = mVeins[i];

		XmlElement* vein = new XmlElement("vein");

		vein->attribute("id", static_cast<int>(i));
		vein->attribute("common_metals", mv[OreType::ORE_COMMON_METALS]);
		vein->attribute("common_minerals", mv[OreType::ORE_COMMON_MINERALS]);
		vein->attribute("rare_metals", mv[OreType::ORE_RARE_METALS]);
		vein->attribute("rare_minerals", mv[OreType::ORE_RARE_MINERALS]);

		element->linkEndChild(vein);
	}
}


/**
 * 
 */
void Mine::deserialize(NAS2D::Xml::XmlElement* element)
{
	int active = 0, yield = 0, depth = 0;
	std::string flags;

	XmlAttribute* attribute = element->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "active") { attribute->queryIntValue(active); }
		else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
		else if (attribute->name() == "yield") { attribute->queryIntValue(yield); }
		else if (attribute->name() == "flags") { mFlags = std::bitset<6>(attribute->value()); }
		attribute = attribute->next();
	}

	this->active(active != 0);
	mProductionRate = static_cast<MineProductionRate>(yield);

	mVeins.resize(static_cast<std::size_t>(depth));
	for (XmlNode* vein = element->firstChild(); vein != nullptr; vein = vein->nextSibling())
	{
		auto _mv = MineVein{0, 0, 0, 0};
		attribute = vein->toElement()->firstAttribute();
		int id = 0;
		while (attribute)
		{
			if (attribute->name() == "common_metals") { attribute->queryIntValue(_mv[OreType::ORE_COMMON_METALS]); }
			else if (attribute->name() == "common_minerals") { attribute->queryIntValue(_mv[OreType::ORE_COMMON_MINERALS]); }
			else if (attribute->name() == "rare_metals") { attribute->queryIntValue(_mv[OreType::ORE_RARE_METALS]); }
			else if (attribute->name() == "rare_minerals") { attribute->queryIntValue(_mv[OreType::ORE_RARE_MINERALS]); }
			else if (attribute->name() == "id") { attribute->queryIntValue(id); }
			attribute = attribute->next();
		}
		mVeins[static_cast<std::size_t>(id)] = _mv;
	}
}
