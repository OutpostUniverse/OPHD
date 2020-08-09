#include "IOHelper.h"

#include "Constants/Strings.h"

#include "StorableResources.h"


void readResources(NAS2D::Xml::XmlElement* element, StorableResources& resources)
{
	if (!element) { throw std::runtime_error("MapViewState::readResources(): Called with element==nullptr"); }

	NAS2D::Xml::XmlAttribute* attribute = element->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == constants::SAVE_GAME_RESOURCE_0) { attribute->queryIntValue(resources.resources[0]); }
		else if (attribute->name() == constants::SAVE_GAME_RESOURCE_1) { attribute->queryIntValue(resources.resources[1]); }
		else if (attribute->name() == constants::SAVE_GAME_RESOURCE_2) { attribute->queryIntValue(resources.resources[2]); }
		else if (attribute->name() == constants::SAVE_GAME_RESOURCE_3) { attribute->queryIntValue(resources.resources[3]); }

		attribute = attribute->next();
	}
}


void writeResources(NAS2D::Xml::XmlElement* element, const StorableResources& resources, const std::string& tagName)
{
	NAS2D::Xml::XmlElement* resources_elem = new NAS2D::Xml::XmlElement(tagName);

	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_0, resources.resources[0]);
	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_1, resources.resources[1]);
	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_2, resources.resources[2]);
	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_3, resources.resources[3]);

	element->linkEndChild(resources_elem);
}
