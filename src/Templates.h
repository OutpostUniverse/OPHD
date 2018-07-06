#pragma once

/**
 * Transfers products from source to destination.
 */
template <class T>
void transferProducts(T& source, T& destination)
{
	if (source->products().empty() || destination->products().atCapacity()) { return; }

	ProductPool::ProductTypeCount& src = source->products().mProducts;

	ProductPool& dest = destination->products();

	for (size_t i = 0; i < PRODUCT_COUNT; ++i)
	{
		if (dest.availableStorage() == 0) { return; }

		int storage_needed = storageRequired(static_cast<ProductType>(i), src[i]);
		int storage_available_at_destination = dest.availableStorage();

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