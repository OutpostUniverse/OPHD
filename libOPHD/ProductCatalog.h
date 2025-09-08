#pragma once

#include "EnumProductType.h"

#include <string>


/**
 * \brief Provides information about Products that can
 *        be manufactured in Factory.
 */
class ProductCatalog
{
public:
	struct Product
	{
		int id;
		std::string name;
		std::string description;
		int storageSpace;
	};

public:
	ProductCatalog() = delete;
	ProductCatalog(const ProductCatalog&) = delete;
	ProductCatalog(const ProductCatalog&&) noexcept = delete;
	ProductCatalog& operator=(const ProductCatalog& other) = delete;
	ProductCatalog& operator=(ProductCatalog&& other) noexcept = delete;

	static void init(const std::string& filename);
	static bool has(ProductType type);
	static const Product& get(ProductType type);
};
