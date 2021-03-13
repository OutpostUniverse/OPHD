#pragma once

#include <NAS2D/Xml/Xml.h>
#include <string>


// Open an xml document
// Throws a runtime_error if the xml is ill formed or the root element name is incorrect
NAS2D::Xml::XmlDocument openXmlFile(std::string filename, std::string rootElementName);
