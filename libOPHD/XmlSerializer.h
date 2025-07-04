#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <stdexcept>


namespace NAS2D::Xml
{
	class XmlDocument;
}


// Open an xml document
// Throws a runtime_error if the xml is ill formed or the root element name is incorrect
NAS2D::Xml::XmlDocument openXmlFile(std::string filename, std::string rootElementName);


template <typename T>
T stringToEnum(const std::unordered_map<std::string, T>& table, std::string value)
{
	auto it = table.find(value);
	if (it != table.end())
	{
		return it->second;
	}

	throw std::runtime_error("Unable to parse enum with value of " + value);
}


template <typename T>
T stringToEnum(const std::map<std::string, T>& table, std::string value)
{
	auto it = table.find(value);
	if (it != table.end())
	{
		return it->second;
	}

	throw std::runtime_error("Unable to parse enum with value of " + value);
}
