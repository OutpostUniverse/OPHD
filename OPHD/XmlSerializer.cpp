#include "XmlSerializer.h"
#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>

using namespace NAS2D;


Xml::XmlDocument openXmlFile(std::string filename, std::string rootElementName)
{
	Xml::XmlDocument xmlDocument;
	xmlDocument.parse(Utility<Filesystem>::get().readFile(filename).c_str());

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
