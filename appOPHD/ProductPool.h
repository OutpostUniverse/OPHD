#pragma once

#include <libOPHD/EnumProductType.h>

#include <array>


namespace NAS2D
{
	class Dictionary;
}


int storageRequiredPerUnit(ProductType type);

class ProductPool
{
public:
	using ProductTypeCount = std::array<int, ProductType::PRODUCT_COUNT>;


	ProductPool();
	~ProductPool() = default;

	ProductPool(const ProductPool&) = default;
	ProductPool& operator=(const ProductPool&) = default;

	int capacity() const;

	int productStorageRequirement(ProductType type) const;

	bool canStore(ProductType type, int count) const;
	bool empty() const;
	bool atCapacity() const;

	void transferAllTo(ProductPool& destination);
	void store(ProductType type, int count);
	int pull(ProductType type, int count);
	int count(ProductType type) const;

	int availableStorage() const;
	int availableStoragePercent() const;

	NAS2D::Dictionary serialize() const;
	void deserialize(const NAS2D::Dictionary& dictionary);

	void verifyCount();

private:
	ProductTypeCount mProducts{};

	int mCapacity;
	int mCurrentStorageCount{0};
};
