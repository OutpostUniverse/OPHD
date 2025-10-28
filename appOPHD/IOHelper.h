#pragma once

#include <string>


namespace NAS2D::Xml
{
	class XmlElement;
}

struct StorableResources;


StorableResources readResourcesOptional(const NAS2D::Xml::XmlElement& parentElement, const std::string& subElementName, const StorableResources& defaultValue);
StorableResources readResourcesOptional(const NAS2D::Xml::XmlElement& parentElement, const std::string& subElementName);
StorableResources readResources(const NAS2D::Xml::XmlElement& parentElement, const std::string& subElementName);
StorableResources readResources(const NAS2D::Xml::XmlElement& element);

NAS2D::Xml::XmlElement* writeResources(const StorableResources&, const std::string&);
