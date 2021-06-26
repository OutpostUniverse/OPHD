#include "IOHelper.h"

#include "Constants/Strings.h"

#include "StorableResources.h"


void readResources(NAS2D::Xml::XmlElement* element, StorableResources& resources)
{
	if (!element) { throw std::runtime_error("MapViewState::readResources(): Called with element==nullptr"); }

	NAS2D::Xml::XmlAttribute* attribute = element->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == constants::SaveGameResource0) { attribute->queryIntValue(resources.resources[0]); }
		else if (attribute->name() == constants::SaveGameResource1) { attribute->queryIntValue(resources.resources[1]); }
		else if (attribute->name() == constants::SaveGameResource2) { attribute->queryIntValue(resources.resources[2]); }
		else if (attribute->name() == constants::SaveGameResource3) { attribute->queryIntValue(resources.resources[3]); }

		attribute = attribute->next();
	}
}


NAS2D::Xml::XmlElement* writeResources(const StorableResources& resources, const std::string& tagName)
{
	NAS2D::Xml::XmlElement* resourcesElement = new NAS2D::Xml::XmlElement(tagName);

	resourcesElement->attribute(constants::SaveGameResource0, resources.resources[0]);
	resourcesElement->attribute(constants::SaveGameResource1, resources.resources[1]);
	resourcesElement->attribute(constants::SaveGameResource2, resources.resources[2]);
	resourcesElement->attribute(constants::SaveGameResource3, resources.resources[3]);

	return resourcesElement;
}
