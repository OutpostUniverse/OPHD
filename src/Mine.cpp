// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "Mine.h"

#include <iostream>

using namespace std;
using namespace NAS2D::Xml;


/**
 * Helper function that gets the total amount of ore 
 */
static int getOreCount(const Mine::MineVeins& veins, Mine::OreType ore, size_t depth)
{
	int _value = 0;
	for (size_t i = 0; i < depth; ++i)
	{
		_value += veins[i][ore];
	}
	return _value;
}


/**
 * Increases the depth of the mine.
 * 
 * \note	This function only modifies the Mine. It has no knowledge of the
 *			maximum digging depth of a planet and doesn't modify any tiles.
 */
void Mine::increaseDepth()
{
	MineVein vein;

	switch (productionRate())
	{
	case PRODUCTION_RATE_LOW:
		vein[ORE_COMMON_METALS] = 150;
		vein[ORE_COMMON_MINERALS] = 125;
		vein[ORE_RARE_METALS] = 150;
		vein[ORE_RARE_MINERALS] = 125;
		break;

	case PRODUCTION_RATE_MEDIUM:
		vein[ORE_COMMON_METALS] = 200;
		vein[ORE_COMMON_MINERALS] = 175;
		vein[ORE_RARE_METALS] = 200;
		vein[ORE_RARE_MINERALS] = 175;
		break;

	case PRODUCTION_RATE_HIGH:
		vein[ORE_COMMON_METALS] = 250;
		vein[ORE_COMMON_MINERALS] = 200;
		vein[ORE_RARE_METALS] = 250;
		vein[ORE_RARE_MINERALS] = 200;
		break;
	}

	mVeins.push_back(vein);
}


/**
 * Gets the current depth of the mine.
 */
int Mine::depth() const
{
	return mVeins.size();
}


/**
 * Convenience function that gets the common metal ore available
 * given the current level.
 */
int Mine::commonMetalsAvailable() const
{
	return getOreCount(mVeins, ORE_COMMON_METALS, depth());
}


/**
 * Convenience function that gets the common mineral ore available
 * given the current level.
 */
int Mine::commonMineralsAvailable() const
{
	return getOreCount(mVeins, ORE_COMMON_MINERALS, depth());
}


/**
 * Convenience function that gets the rare metal ore available
 * given the current level.
 */
int Mine::rareMetalsAvailable() const
{
	return getOreCount(mVeins, ORE_RARE_METALS, depth());
}


/**
 * Convenience function that gets the rare mineral ore available
 * given the current level.
 */
int Mine::rareMineralsAvailable() const
{
	return getOreCount(mVeins, ORE_RARE_MINERALS, depth());
}


/**
 * Indicates that there are no resources available at this mine.
 */
bool Mine::exhausted() const
{
	int ore_count = 0;

	for (auto vein : mVeins)
	{
		ore_count += vein[ORE_COMMON_METALS];
		ore_count += vein[ORE_COMMON_MINERALS];
		ore_count += vein[ORE_RARE_METALS];
		ore_count += vein[ORE_RARE_MINERALS];
	}

	return (ore_count == 0);
}


/**
 * Serializes current mine information.
 */
void Mine::serialize(XmlElement* _ti)
{
	_ti->attribute("depth", depth());
	_ti->attribute("active", active());
	_ti->attribute("yield", productionRate());

	for (size_t i = 0; i < mVeins.size(); ++i)
	{
		const MineVein& mv = mVeins[i];

		XmlElement* vein = new XmlElement("vein");

		vein->attribute("id", static_cast<int>(i));
		vein->attribute("common_metals",	mv[ORE_COMMON_METALS]);
		vein->attribute("common_minerals",	mv[ORE_COMMON_MINERALS]);
		vein->attribute("rare_metals",		mv[ORE_RARE_METALS]);
		vein->attribute("rare_minerals",	mv[ORE_RARE_MINERALS]);

		_ti->linkEndChild(vein);
	}
}


/**
 * 
 */
void Mine::deserialize(NAS2D::Xml::XmlElement* _ti)
{
	int active = 0, yield = 0, depth = 0;

	XmlAttribute* attribute = _ti->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == "active") { attribute->queryIntValue(active); }
		else if (attribute->name() == "depth") { attribute->queryIntValue(depth); }
		else if (attribute->name() == "yield") { attribute->queryIntValue(yield); }
		attribute = attribute->next();
	}

	this->active(active != 0);
	mProductionRate = static_cast<MineProductionRate>(yield);

	mVeins.resize(depth);
	for (XmlNode* vein = _ti->firstChild(); vein != nullptr; vein = vein->nextSibling())
	{
		MineVein _mv;
		attribute = vein->toElement()->firstAttribute();
		int id = 0;
		while (attribute)
		{
			if (attribute->name() == "common_metals") { attribute->queryIntValue(_mv[ORE_COMMON_METALS]); }
			else if (attribute->name() == "common_minerals") { attribute->queryIntValue(_mv[ORE_COMMON_MINERALS]); }
			else if (attribute->name() == "rare_metals") { attribute->queryIntValue(_mv[ORE_RARE_METALS]); }
			else if (attribute->name() == "rare_minerals") { attribute->queryIntValue(_mv[ORE_RARE_MINERALS]); }
			else if (attribute->name() == "id") { attribute->queryIntValue(id); }
			attribute = attribute->next();
		}
		mVeins[id] = _mv;
	}
}
