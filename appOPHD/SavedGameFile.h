#pragma once

#include <NAS2D/Xml/XmlDocument.h>

#include <string>


namespace NAS2D::Xml
{
	class XmlElement;
}

class ColonyShip;


class SavedGameFile
{
public:
	SavedGameFile();
	explicit SavedGameFile(const std::string& filePath);

	const NAS2D::Xml::XmlDocument& savedGameDocument() const;
	NAS2D::Xml::XmlDocument& savedGameDocument();

	const NAS2D::Xml::XmlElement& root() const;
	NAS2D::Xml::XmlElement& root();

	ColonyShip colonyShip() const;

private:
	NAS2D::Xml::XmlDocument mSavedGameDocument;
	NAS2D::Xml::XmlElement& mRoot;
};
