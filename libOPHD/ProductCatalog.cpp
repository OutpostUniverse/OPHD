#include "ProductCatalog.h"

#include "XmlSerializer.h"

#include <NAS2D/ParserHelper.h>
#include <NAS2D/Xml/XmlDocument.h>
#include <NAS2D/Xml/XmlElement.h>

#include <map>
#include <stdexcept>


namespace
{
	std::map<ProductType, ProductCatalog::Product> mProductTable;


	ProductCatalog::Product parseProduct(const NAS2D::Xml::XmlElement* node)
	{
		const auto dictionary = NAS2D::attributesToDictionary(*node);

		const auto requiredFields = std::vector<std::string>{"id", "name", "description", "storageSpace"};
		NAS2D::reportMissingOrUnexpected(dictionary.keys(), requiredFields, {});

		return {
			dictionary.get<int>("id"),
			dictionary.get("name"),
			dictionary.get("description"),
			dictionary.get<int>("storageSpace"),
		};
	}


	std::map<ProductType, ProductCatalog::Product> loadProductTypes(const std::string& filename)
	{
		std::map<ProductType, ProductCatalog::Product> productTable;

		const std::string RootElementName("products");
		const std::string ProductElement("product");

		auto xmlDocument = openXmlFile(filename, RootElementName);
		auto rootElement = xmlDocument.firstChildElement(RootElementName);
		for (const auto* node = rootElement->firstChildElement(ProductElement); node; node = node->nextSiblingElement(ProductElement))
		{
			const auto product = parseProduct(node);
			const auto productId = static_cast<ProductType>(product.id);

			if (productTable.contains(productId))
			{
				throw std::runtime_error("Duplicate ProductID in data file: ID = " + std::to_string(product.id) + ", Name = " + product.name + ", filename = " + filename);
			}

			productTable.try_emplace(productId, product);
		}

		return productTable;
	}
}


void ProductCatalog::init(const std::string& filename)
{
	mProductTable = loadProductTypes(filename);
}


bool ProductCatalog::has(ProductType type)
{
	return mProductTable.contains(type);
}


const ProductCatalog::Product& ProductCatalog::get(ProductType type)
{
	try
	{
		return mProductTable.at(type);
	}
	catch (std::out_of_range&)
	{
		throw std::runtime_error("Product type not found in Product Catalogue: " + std::to_string(static_cast<int>(type)) + " of " + std::to_string(mProductTable.size()));
	}
}
