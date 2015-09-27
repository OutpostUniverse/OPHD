#ifndef __THING_CATALOG__
#define __THING_CATALOG__

#include "NAS2D/NAS2D.h"

#include <map>
#include <string>


#include "Thing.h"

class ThingCatalog
{
public:

	ThingCatalog(const std::string& catalog);
	~ThingCatalog();

	void save();

	Thing thing(int id);
	Thing thing(const std::string& catagory, const std::string& name);

	StringList catagories();
	StringList things_in_catagory(const std::string& catagory);


private:

	void readCatalog();
	void processThing(TiXmlNode* node);

	struct ThingInfo
	{
		ThingInfo(	const std::string& img_source,
					const std::string& name,
					const std::string& catagory,
					const Rectangle_2d& rect,
					const Point_2d& anchor):
											image(img_source),
											name(name),
											catagory(catagory),
											imageRect(rect),
											anchor(anchor)
		{}

		ThingInfo()
		{}

		Image			image;
		std::string		name;
		std::string		catagory;
		Rectangle_2d	imageRect;
		Point_2d		anchor;

	};

	typedef std::map<int, ThingInfo> ThingTable;

	std::string		mCatalogFile;

	ThingTable		mThingTable;

};



#endif