#pragma once

#include "Technology.h"

#include <string>
#include <vector>

#include <NAS2D/Dictionary.h>
#include <NAS2D/Xml/Xml.h>


class TechnologyCatalog
{
public:
	TechnologyCatalog() = delete;
	TechnologyCatalog(const std::string& techFile);

	const std::vector<Technology>& technologies() const { return mTechnologies; }
	const std::vector<std::string>& categories() const { return mCategories; }

	const Technology& technologyFromId(int id) const;

private:
	std::vector<Technology> mTechnologies;
	std::vector<std::string> mCategories;

	void readCategories(NAS2D::Xml::XmlElement& node);
	void readTechnologiesInCategory(NAS2D::Xml::XmlElement& category);
	void readTechnology(NAS2D::Xml::XmlElement& technology, const NAS2D::Dictionary& attributes);
	void readEffects(NAS2D::Xml::XmlElement& effects, Technology& technology);
};
