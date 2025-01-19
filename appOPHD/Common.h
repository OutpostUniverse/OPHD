#pragma once

#include "EnumProductType.h"

#include <NAS2D/Math/Rectangle.h>

#include <string>
#include <vector>

namespace NAS2D::Xml
{
	class XmlDocument;
}

void checkSavegameVersion(const std::string& filename);
NAS2D::Xml::XmlDocument openSavegame(const std::string& filename);

void setMeanSolarDistance(float newMeanSolarDistance);
float getMeanSolarDistance();


std::string formatDiff(int diff);
