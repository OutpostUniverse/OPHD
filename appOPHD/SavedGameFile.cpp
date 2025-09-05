#include "SavedGameFile.h"

#include "States/ColonyShip.h"

#include <libOPHD/XmlSerializer.h>

#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlElement.h>

#include <stdexcept>


namespace
{
	const std::string saveGameVersionSupported = "0.31";
	const std::string saveGameRootNode = "OutpostHD_SaveGame";


	ColonyShipLanders readLanders(NAS2D::Xml::XmlElement* element)
	{
		if (!element) { return {}; }

		const auto dictionary = NAS2D::attributesToDictionary(*element);
		return {
			.colonist = dictionary.get<int>("colonist_landers"),
			.cargo = dictionary.get<int>("cargo_landers"),
		};
	}


	int readTurnCount(NAS2D::Xml::XmlElement* element)
	{
		return (element) ? NAS2D::attributesToDictionary(*element).get<int>("count") : 0;
	}
}


SavedGameFile::SavedGameFile() :
	mRoot{*NAS2D::dictionaryToAttributes(saveGameRootNode, {{{"version", saveGameVersionSupported}}})}
{
	mSavedGameDocument.linkEndChild(&mRoot);
}


SavedGameFile::SavedGameFile(const std::string& filePath) :
	mSavedGameDocument{openXmlFile(filePath, saveGameRootNode)},
	mRoot{*mSavedGameDocument.firstChildElement(saveGameRootNode)}
{
	auto saveGameVersion = mRoot.attribute("version");
	if (saveGameVersion != saveGameVersionSupported)
	{
		throw std::runtime_error("Save game version mismatch: " + filePath + " : Version = " + saveGameVersion + " (Supported = " + saveGameVersionSupported + ")");
	}
}


const NAS2D::Xml::XmlDocument& SavedGameFile::savedGameDocument() const
{
	return mSavedGameDocument;
}


NAS2D::Xml::XmlDocument& SavedGameFile::savedGameDocument()
{
	return mSavedGameDocument;
}


const NAS2D::Xml::XmlElement& SavedGameFile::root() const
{
	return mRoot;
}


NAS2D::Xml::XmlElement& SavedGameFile::root()
{
	return mRoot;
}


ColonyShip SavedGameFile::colonyShip() const
{
	return {
		readLanders(mRoot.firstChildElement("population")),
		readTurnCount(mRoot.firstChildElement("turns")),
	};
}
