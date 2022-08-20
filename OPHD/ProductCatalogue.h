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
	ProductCatalogue(const std::string& catalogFile);

	const Product& product(ProductType type) const;

private:
	std::map<ProductType, Product> mProductTable;
};
