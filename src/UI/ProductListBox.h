#pragma once

#include "NAS2D/NAS2D.h"
#include "Core/ListBoxBase.h"

class ProductPool;

/**
 * Specialized ListBox to display a list of products in a ProductPool.
 */
class ProductListBox : public ListBoxBase
{
public:
	class ProductListBoxItem : public ListBoxItem
	{
	public:
		ProductListBoxItem() = default;
		virtual ~ProductListBoxItem() = default;

	public:
		size_t count = 0;				/**< Count of the product. */
		float usage = 0.0f;				/**< Usage of available capacity. */
	};

public:
	ProductListBox();
	virtual ~ProductListBox() = default;

	void productPool(ProductPool&);

	virtual void update() final;

private:
	void _init();

private:
	using ProductItemList = std::vector<ProductListBoxItem>;
};
