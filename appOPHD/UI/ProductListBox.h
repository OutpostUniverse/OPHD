#pragma once

#include <libControls/ListBoxBase.h>

#include <vector>


class ProductPool;


/**
 * Specialized ListBox to display a list of products in a ProductPool.
 */
class ProductListBox : public ListBoxBase
{
public:
	struct ProductListBoxItem
	{
		std::string text;
		int productCount = 0;
		int capacityUsed = 0;
		int capacityTotal = 0;
	};

	ProductListBox(SelectionChangedDelegate selectionChangedHandler = {});

	void productPool(const ProductPool&);

	void clear();

protected:
	void add(std::string initialText, int initialProductCount, int initialCapacityUsed, int initialCapacityTotal);

	std::size_t count() const override;

	const ProductListBoxItem& getItem(std::size_t index) const;

	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override;

private:
	std::vector<ProductListBoxItem> mItems;
};
