#pragma once


#include "Common.h"

#include <map>
#include <string>


/**
 * \brief Provides information about Products that can
 *        be manufactured in Factory.
 */
class ProductCatalogue
{
public:
	struct Product
	{
		int Id{ProductType::PRODUCT_NONE};
		std::string Name{};
		std::string Description{};
	};

public:
	ProductCatalogue() = delete;
	ProductCatalogue(const ProductCatalogue&) = delete;
	ProductCatalogue(const ProductCatalogue&&) noexcept = delete;
	ProductCatalogue& operator=(const ProductCatalogue& other) = delete;
	ProductCatalogue& operator=(ProductCatalogue&& other) noexcept = delete;

	static void init(const std::string& filename);
	static const Product& get(ProductType type);

private:
	static std::map<ProductType, Product> mProductTable;
};
