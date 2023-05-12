#include "IOHelper.h"

#include "Constants/Strings.h"

#include "StorableResources.h"

#include <NAS2D/ParserHelper.h>


StorableResources readResources(const NAS2D::Xml::XmlElement* element)
{
	if (!element) { throw std::runtime_error("MapViewState::readResources(): Called with element==nullptr"); }

	const auto dictionary = NAS2D::attributesToDictionary(*element);

	const auto requiredFields = std::vector<std::string>{constants::SaveGameResource0, constants::SaveGameResource1, constants::SaveGameResource2, constants::SaveGameResource3};
	NAS2D::reportMissingOrUnexpected(dictionary.keys(), requiredFields, {});

	return StorableResources{{
		dictionary.get<int>(constants::SaveGameResource0),
		dictionary.get<int>(constants::SaveGameResource1),
		dictionary.get<int>(constants::SaveGameResource2),
		dictionary.get<int>(constants::SaveGameResource3),
	}};
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
