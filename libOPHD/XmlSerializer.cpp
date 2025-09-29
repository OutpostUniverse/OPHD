#include "XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/Xml/XmlDocument.h>


NAS2D::Xml::XmlDocument openXmlFile(std::string filename, std::string rootElementName)
{
	NAS2D::Xml::XmlDocument xmlDocument;
	xmlDocument.parse(NAS2D::Utility<NAS2D::Filesystem>::get().readFile(NAS2D::VirtualPath{filename}).c_str());

	if (xmlDocument.error())
	{
		throw std::runtime_error(filename + " has malformed XML: Row: " + std::to_string(xmlDocument.errorRow()) +
			" Column: " + std::to_string(xmlDocument.errorCol()) + " : " + xmlDocument.errorDesc());
	}

	const auto* xmlRootElement = xmlDocument.firstChildElement(rootElementName);
	if (!xmlRootElement) {
		throw std::runtime_error(filename + " does not contain required root tag of <" + rootElementName + ">");
	}

	return xmlDocument;
}
