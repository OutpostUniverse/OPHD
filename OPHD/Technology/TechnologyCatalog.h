#pragma once

#include "Technology.h"

#include <map>
#include <string>
#include <vector>

#include <NAS2D/Dictionary.h>
#include <NAS2D/Xml/Xml.h>


class TechnologyCatalog
{
public:
	TechnologyCatalog() = delete;
	TechnologyCatalog(const std::string& techFile);

	const Technology& technologyFromId(int id) const;

	const std::vector<Technology> technologiesInCategory(const std::string& categoryName) const;

private:
	std::map<std::string, std::vector<Technology>> mCategories;

	void readCategories(NAS2D::Xml::XmlElement& node);
	void readTechnologiesInCategory(const std::string& categoryName, NAS2D::Xml::XmlElement& category);
	void readTechnology(const std::string& categoryName, NAS2D::Xml::XmlElement& technology, const NAS2D::Dictionary& attributes);
	void readEffects(NAS2D::Xml::XmlElement& effects, Technology& technology);
};
