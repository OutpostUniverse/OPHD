#pragma once

#include "Common.h"
#include "Constants.h"

#include <array>

class ProductPool
{
public:
	ProductPool() = default;
	~ProductPool() = default;

	int capacity() const;

	bool canStore(ProductType type, int count);

	bool empty() const { return (capacity() == 0); }

	void store(ProductType type, int count);
	int pull(ProductType type, int count);
	int count(ProductType type);

private:
	typedef std::array<int, PRODUCT_COUNT>	ProductTypeCount;

private:
	ProductPool(const ProductPool&) = delete;
	ProductPool& operator=(const ProductPool&) = delete;

	int _availableStorage() const;

private:
	ProductTypeCount	mProducts = {{ 0 }};

	int					mCapacity = constants::BASE_PRODUCT_CAPACITY;
};
