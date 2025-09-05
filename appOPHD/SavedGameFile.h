#pragma once

#include <NAS2D/Xml/XmlDocument.h>

#include <string>


namespace NAS2D::Xml
{
	class XmlElement;
}


class SavedGameFile
{
public:
	SavedGameFile();
	explicit SavedGameFile(const std::string& filePath);

	const NAS2D::Xml::XmlDocument& savedGameDocument() const;
	NAS2D::Xml::XmlDocument& savedGameDocument();

	const NAS2D::Xml::XmlElement& root() const;
	NAS2D::Xml::XmlElement& root();

private:
	NAS2D::Xml::XmlDocument mSavedGameDocument;
	NAS2D::Xml::XmlElement& mRoot;
};
