#pragma once

#include <NAS2D/Xml/Xml.h>
#include <string>

struct StorableResources;

void readResources(NAS2D::Xml::XmlElement* element, StorableResources& resources);

NAS2D::Xml::XmlElement* writeResources(const StorableResources&, const std::string&);
