#pragma once

#include "Common.h"
#include "Constants.h"
#include "Templates.h"

#include <array>


class ProductPool
{
public:
	typedef std::array<int, PRODUCT_COUNT>	ProductTypeCount;

public:
	ProductPool() = default;
	~ProductPool() = default;

	int capacity() const;

	bool canStore(ProductType type, int count);
	bool empty() const;
	bool atCapacity() const;

	void store(ProductType type, int count);
	int pull(ProductType type, int count);
	int count(ProductType type);

	int availableStorage() const;

	void serialize(NAS2D::Xml::XmlElement* _ti);
	void deserialize(NAS2D::Xml::XmlElement* _ti);

	void verifyCount();

private:
	template <class T>
	friend void transferProducts(T& source, T& destination);

private:
	ProductPool(const ProductPool&) = delete;
	ProductPool& operator=(const ProductPool&) = delete;

private:
	ProductTypeCount	mProducts = {{ 0 }};

	int					mCapacity = constants::BASE_PRODUCT_CAPACITY;
	int					mCurrentStorageCount = 0;
};
