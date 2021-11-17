#include "TechnologyReader.h"
#include "../XmlSerializer.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Filesystem.h>
#include <NAS2D/Xml/Xml.h>

#include <stdexcept>

using namespace NAS2D;

TechnologyReader::TechnologyReader(const std::string& techFile)
{
	Xml::XmlDocument xmlDocument = openXmlFile(techFile, "technology");
}
