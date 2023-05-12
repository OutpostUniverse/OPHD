#pragma once

#include <NAS2D/Xml/Xml.h>
#include <string>

struct StorableResources;


StorableResources readResources(const NAS2D::Xml::XmlElement* element);

NAS2D::Xml::XmlElement* writeResources(const StorableResources&, const std::string&);
