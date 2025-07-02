#pragma once

#include <string>
#include <vector>


struct Technology;


class TechnologyCatalog
{
public:
	struct Category
	{
		const int icon_index{0};
		const std::string name;
		std::vector<Technology> technologies;
	};

public:
	TechnologyCatalog() = delete;
	TechnologyCatalog(const std::string& techFile);

	const std::vector<std::string> categoryNames();

	const Technology& technologyFromId(int id) const;

	const std::vector<Technology>& technologiesInCategory(const std::string& categoryName) const;

	const std::vector<Category>& categories() { return mCategories; }

private:
	std::vector<Category> mCategories;
};
