#pragma once

#include <libControls/ListBoxBase.h>


namespace NAS2D
{
	class Font;
}

class ProductPool;


/**
 * Specialized ListBox to display a list of products in a ProductPool.
 */
class ProductListBox : public ListBoxBase
{
public:
	struct ProductListBoxItem : public ListBoxItem
	{
		int count = 0; /**< Count of the product. */
		int capacityUsed = 0;
		int capacityTotal = 0;
	};

	ProductListBox();

	void productPool(ProductPool&);

	void update() override;
};
