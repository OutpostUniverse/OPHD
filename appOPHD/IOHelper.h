#pragma once

#include <NAS2D/Xml/Xml.h>
#include <string>

struct StorableResources;


StorableResources readResourcesOptional(const NAS2D::Xml::XmlElement& parentElement, const std::string& subElementName, const StorableResources& defaultValue);
StorableResources readResourcesOptional(const NAS2D::Xml::XmlElement& parentElement, const std::string& subElementName);
StorableResources readResources(const NAS2D::Xml::XmlElement& parentElement, const std::string& subElementName);
StorableResources readResources(const NAS2D::Xml::XmlElement& element);

NAS2D::Xml::XmlElement* writeResources(const StorableResources&, const std::string&);
