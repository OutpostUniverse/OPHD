#pragma once

#include <NAS2D/StringUtils.h>
#include <NAS2D/Xml/Xml.h>
#include <string>
#include <unordered_map>
#include <typeinfo>
#include <type_traits>
#include <stdexcept>


// Open an xml document
// Throws a runtime_error if the xml is ill formed or the root element name is incorrect
NAS2D::Xml::XmlDocument openXmlFile(std::string filename, std::string rootElementName);

int elementToInt(const NAS2D::Xml::XmlElement* element);

template<typename T>
T stringToEnum(const std::unordered_map<std::string, T>& table, std::string value)
{
	auto it = table.find(NAS2D::toLowercase(value));
	if (it != table.end()) {
		return it->second;
	}

	throw std::runtime_error("Unable to parse enum with value of " + value);
}


// Create an overload to parse specific enums
template<typename T, std::enable_if_t<!std::is_enum<T>::value, bool> = true>
void parseElementValue(T& destination, const NAS2D::Xml::XmlElement* element)
{
	if constexpr (std::is_same_v<T, int>) {
		destination = elementToInt(element);
	}
	else if constexpr (std::is_same_v<T, float>) {
		destination = static_cast<float>(std::atof(element->getText().c_str()));
	}
	else if constexpr (std::is_same_v<T, double>) {
		destination = std::atof(element->getText().c_str());
	}
	else if constexpr (std::is_same_v<T, std::string>) {
		destination = element->getText();
	}
	else {
		throw std::logic_error("Unable to parse type " + std::string(typeid(T).name()) + " from XML");
	}
}
