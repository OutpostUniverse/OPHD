#pragma once

#include <NAS2D/Xml/Xml.h>
#include <string>

class StorableResources;

void writeResources(NAS2D::Xml::XmlElement*, const StorableResources&, const std::string&);