#include "ThingCatalog.h"

#include <set>

ThingCatalog::ThingCatalog(const std::string& catalog):	mCatalogFile(catalog)
{
	readCatalog();
}


ThingCatalog::~ThingCatalog()
{}


Thing ThingCatalog::thing(int id)
{
	ThingTable::iterator it = mThingTable.find(id);

	if(it == mThingTable.end())
		return Thing();

	Thing _t(it->second.image);
	_t.name(it->second.name);
	_t.imageRect(it->second.imageRect);
	_t.anchor(it->second.anchor);

	return _t;
}


Thing ThingCatalog::thing(const string& catagory, const string& name)
{
	vector<ThingInfo*> thingInfoSet;

	for(ThingTable::iterator it = mThingTable.begin(); it != mThingTable.end(); it++)
	{
		if(it->second.catagory == catagory)
			thingInfoSet.push_back(&it->second);
	}

	for(size_t i = 0; i < thingInfoSet.size(); i++)
	{
		if(thingInfoSet[i]->name == name)
		{
			Thing _t(thingInfoSet[i]->image);
			_t.name(thingInfoSet[i]->name);
			_t.imageRect(thingInfoSet[i]->imageRect);
			_t.anchor(thingInfoSet[i]->anchor);

			return _t;
		}
	}

	return Thing();
}


StringList ThingCatalog::catagories()
{
	StringList strList;

	set<string> strSet;

	for(ThingTable::iterator it = mThingTable.begin(); it != mThingTable.end(); it++)
		strSet.insert(it->second.catagory);

	for(set<string>::iterator it = strSet.begin(); it != strSet.end(); it++)
		strList.push_back(*it);

	return strList;
}


/**
 * Gets a list of thing names under a given catagory.
 */
StringList ThingCatalog::things_in_catagory(const string& catagory)
{
	vector<ThingInfo*> thingInfoSet;

	for(ThingTable::iterator it = mThingTable.begin(); it != mThingTable.end(); it++)
	{
		if(it->second.catagory == catagory)
			thingInfoSet.push_back(&it->second);
	}

	StringList strList;
	for(size_t i = 0; i < thingInfoSet.size(); i++)
		strList.push_back(thingInfoSet[i]->name);

	return strList;
}


/**
 * Saves an XML catalog file.
 */
void ThingCatalog::save()
{
}


/**
 * Reads an XML catalog file.
 */
void ThingCatalog::readCatalog()
{
	File xmlFile = Utility<Filesystem>::get().open(mCatalogFile);

	TiXmlDocument doc;
	TiXmlElement *root;

	// Load the XML document and handle any errors that occur.
	doc.Parse(xmlFile.raw_bytes());
	if(doc.Error())
	{
		cout << "Things file '" << mCatalogFile << "' is empty." << endl;
		return;
	}
	else
	{
		// Find the 'things' tag. Barf if not found.
		root = doc.FirstChildElement("things");
		if(root == 0)
		{
			cout << "Root element in '" << mCatalogFile << "' is not 'things'." << endl;
			return;
		}

		// Start parsing through the catalog file.
		TiXmlNode* node = 0;
		while(node = root->IterateChildren(node))
		{
			if(node->ValueStr() == "thing")
			{
				processThing(node);
			}
			else
			{
				cout << "Unexpected tag '<" << node->ValueStr() << ">' found in '" << mCatalogFile << "' on row " << node->Row() << "." << endl;
			}
		}
	}
}


/**
 * Proceses an XML Thing definition.
 * 
 * \note	Thing definition processor will only read the first 'image'
 *			child node of a Thing. Any additional image tags will be
 *			ignored.
 * 
 * \todo	At some point in the future I'd like to provide a way to
 *			define animated things.
 */
void ThingCatalog::processThing(TiXmlNode* node)
{
	XmlAttributeParser parser;

	int id = parser.intAttribute(node, "id");
	string name = parser.stringAttribute(node, "name");
	string catagory = parser.stringAttribute(node, "catagory");
	string img_source;

	Rectangle_2d rect;
	Point_2d anchor;

	// Read the first child node of 'thing'. Ignore all the rest.
	TiXmlNode *child = node->FirstChild();
	if(child)
	{
		if(child->ValueStr() == "image")
		{
			img_source = parser.stringAttribute(child, "source");
			rect(parser.intAttribute(child, "x"), parser.intAttribute(child, "y"), parser.intAttribute(child, "w"), parser.intAttribute(child, "h"));
			anchor(parser.intAttribute(child, "anchor_x"), parser.intAttribute(child, "anchor_y"));
		}
		else
		{
			// Only supports '<image>' child nodes, barf on anything else.
			cout << "Unexpected tag '<" << child->ValueStr() << ">' found in thing catalog '" << mCatalogFile << "' on row " << child->Row() << "." << endl;
		}
	}

	if(!catagory.empty())
	{
		mThingTable[id] = ThingInfo(img_source, name, catagory, rect, anchor);
	}
}