#include "XmlSerializer.h"
#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>

using namespace NAS2D;


Xml::XmlDocument openXmlFile(std::string filename, std::string rootElementName)
{
	Xml::XmlDocument xmlDocument;
	xmlDocument.parse(Utility<Filesystem>::get().open(filename).raw_bytes());

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


int elementToInt(const NAS2D::Xml::XmlElement* element)
{
	try
	{
		return std::stoi(element->getText());
	}
	catch (const std::invalid_argument&)
	{
		throw std::runtime_error("Unable to convert value " + element->getText() +
			" from XML element " + element->value() + " into an integer");
	}
	catch (const std::out_of_range&)
	{
		throw std::runtime_error("Value from XML element " + element->value() + " is out of range for an integer");
	}
}
