#pragma once

#include "Technology.h"

#include <map>
#include <string>
#include <vector>


class TechnologyCatalog
{
public:
	TechnologyCatalog() = delete;
	TechnologyCatalog(const std::string& techFile);

	const std::vector<std::string>& categories() const { return mCategorNames; }

	const Technology& technologyFromId(int id) const;

	const std::vector<Technology> technologiesInCategory(const std::string& categoryName) const;

private:
	std::vector<std::string> mCategorNames;
	std::map<std::string, std::vector<Technology>> mCategories;
};
