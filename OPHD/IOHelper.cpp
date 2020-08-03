#include "IOHelper.h"

#include "Constants/Strings.h"

#include "StorableResources.h"

/**
 * 
 */
void writeResources(NAS2D::Xml::XmlElement* element, const StorableResources& resources, const std::string& tagName)
{
	NAS2D::Xml::XmlElement* resources_elem = new NAS2D::Xml::XmlElement(tagName);

	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_0, resources.resources[0]);
	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_1, resources.resources[1]);
	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_2, resources.resources[2]);
	resources_elem->attribute(constants::SAVE_GAME_RESOURCE_3, resources.resources[3]);

	element->linkEndChild(resources_elem);
}
