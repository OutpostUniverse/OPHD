#pragma once

#include "Constants.h"

int storageRequired(ProductType type, int count);
int storageRequiredPerUnit(ProductType type);

/**
 * Transfers products from source to destination.
 */
template <class T>
void transferProducts(T& source, T& destination)
{
	if (source->products().empty() || destination->products().atCapacity()) { return; }

	auto& src = source->products().mProducts;
	auto& dest = destination->products();

	for (size_t i = 0; i < PRODUCT_COUNT; ++i)
	{
		if (dest.availableStorage() == 0) { return; }

		if (dest.availableStorage() >= storageRequired(static_cast<ProductType>(i), src[i]))
		{
			dest.store(static_cast<ProductType>(i), src[i]);
			source->products().pull(static_cast<ProductType>(i), src[i]);
		}
		else
		{
			int units_to_move = dest.availableStorage() / storageRequiredPerUnit(static_cast<ProductType>(i));
			dest.store(static_cast<ProductType>(i), units_to_move);
			source->products().pull(static_cast<ProductType>(i), units_to_move);
		}
	}
}