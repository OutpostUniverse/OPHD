#pragma once

#include "Common.h"
#include "Constants.h"
#include "Templates.h"

#include <NAS2D/Xml/XmlElement.h>

#include <array>


class ProductPool
{
public:
	typedef std::array<int, static_cast<std::size_t>(ProductType::PRODUCT_COUNT)>	ProductTypeCount;

public:
	ProductPool() = default;
	~ProductPool() = default;

	ProductPool(const ProductPool&) = default;
	ProductPool& operator=(const ProductPool&) = default;

public:
	int capacity() const;

	bool canStore(ProductType type, int count);
	bool empty() const;
	bool atCapacity() const;

	void store(ProductType type, int count);
	int pull(ProductType type, int count);
	int count(ProductType type);

	int availableStorage() const;

	void serialize(NAS2D::Xml::XmlElement* element);
	void deserialize(NAS2D::Xml::XmlElement* element);

	void verifyCount();

private:
	template <class T>
	friend void transferProductsStructure(T&, T&);
	friend void transferProductsPool(ProductPool&, ProductPool&);

private:
	ProductTypeCount	mProducts = {{ 0 }};

	int					mCapacity = constants::BASE_PRODUCT_CAPACITY;
	int					mCurrentStorageCount = 0;
};
