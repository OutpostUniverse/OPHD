#include "ProductCatalogue.h"

#include "XmlSerializer.h"

#include <NAS2D/ParserHelper.h>

#include <iostream>


namespace
{
	ProductCatalogue::Product parseProduct(const NAS2D::Xml::XmlElement* node)
	{
		NAS2D::Dictionary dictionary{};
		for (const auto* element = node->firstChildElement(); element; element = element->nextSiblingElement())
		{
			dictionary.set(element->value(), element->getText());
		}

		const auto requiredFields = std::vector<std::string>{"id", "name", "description"};
		
		NAS2D::reportMissingOrUnexpected(dictionary.keys(), requiredFields, {});

		return {
			dictionary.get<int>("id"),
			dictionary.get("name"),
			dictionary.get("description")
		};
	}
}


ProductCatalogue::ProductCatalogue(const std::string& catalogFile)
{
	const std::string RootElementName("products");
	const std::string ProductElement("product");
	
	auto xmlDocument = openXmlFile(catalogFile, RootElementName);
	auto rootElement = xmlDocument.firstChildElement(RootElementName);
	for (const auto* node = rootElement->firstChildElement(ProductElement); node; node = node->nextSiblingElement(ProductElement))
	{
		const auto product = parseProduct(node);
		const auto productId = static_cast<ProductType>(product.Id);

		if (mProductTable.find(productId) != mProductTable.end())
		{
			std::cout << "ProductID (" << product.Id << ") already found in table: " <<
				mProductTable[productId].Name << ". Previous entry will be overwritten with new entry '" <<
				product.Name << "'" << std::endl;
		}

		mProductTable[productId] = product;
	}
}


const ProductCatalogue::Product& ProductCatalogue::product(ProductType type) const
{
	const auto& product = mProductTable.find(type);
	if (product != mProductTable.end())
	{
		return product->second;
	}
	else
	{
		throw std::runtime_error("Unknown ProductType: " + std::to_string(static_cast<int>(type)));
	}
}
