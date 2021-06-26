#include "IOHelper.h"

#include "Constants/Strings.h"

#include "StorableResources.h"

#include <NAS2D/ParserHelper.h>


StorableResources readResources(NAS2D::Xml::XmlElement* element)
{
	if (!element) { throw std::runtime_error("MapViewState::readResources(): Called with element==nullptr"); }

	StorableResources resources{};
	NAS2D::Xml::XmlAttribute* attribute = element->firstAttribute();
	while (attribute)
	{
		if (attribute->name() == constants::SaveGameResource0) { attribute->queryIntValue(resources.resources[0]); }
		else if (attribute->name() == constants::SaveGameResource1) { attribute->queryIntValue(resources.resources[1]); }
		else if (attribute->name() == constants::SaveGameResource2) { attribute->queryIntValue(resources.resources[2]); }
		else if (attribute->name() == constants::SaveGameResource3) { attribute->queryIntValue(resources.resources[3]); }

		attribute = attribute->next();
	}
	return resources;
}


NAS2D::Xml::XmlElement* writeResources(const StorableResources& resources, const std::string& tagName)
{
	return NAS2D::dictionaryToAttributes(
		tagName,
		{{
			{constants::SaveGameResource0, resources.resources[0]},
			{constants::SaveGameResource1, resources.resources[1]},
			{constants::SaveGameResource2, resources.resources[2]},
			{constants::SaveGameResource3, resources.resources[3]},
		}}
	);
}
