#pragma once


#include "Common.h"

#include <string>


/**
 * \brief Provides information about Products that can
 *        be manufactured in Factory.
 */
class ProductCatalogue
{
public:
	ProductCatalogue() = default;

	const std::string& productName(ProductType type) const;

private:

};
