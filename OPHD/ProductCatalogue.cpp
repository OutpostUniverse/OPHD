#include "ProductCatalogue.h"


namespace
{
	const std::string DefaultProductName = "Default Product";
}


const std::string& ProductCatalogue::productName(ProductType type) const
{
	return DefaultProductName;
}
