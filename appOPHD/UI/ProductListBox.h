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
	struct ProductListBoxItem
	{
		ProductListBoxItem(std::string initialText, int initialProductCount, int initialCapacityUsed, int initialCapacityTotal) :
			text{initialText},
			productCount{initialProductCount},
			capacityUsed{initialCapacityUsed},
			capacityTotal{initialCapacityTotal}
		{}

		std::string text;
		int productCount = 0;
		int capacityUsed = 0;
		int capacityTotal = 0;
	};

	ProductListBox();

	void productPool(const ProductPool&);

	void clear();

protected:
	void add(std::string initialText, int initialProductCount, int initialCapacityUsed, int initialCapacityTotal) {
		mItems.emplace_back(ProductListBoxItem{initialText, initialProductCount, initialCapacityUsed, initialCapacityTotal});
		updateScrollLayout();
	}

	virtual std::size_t count() const override;

	const ProductListBoxItem& getItem(std::size_t index) const;

	void drawItem(NAS2D::Renderer& renderer, NAS2D::Rectangle<int> drawArea, std::size_t index) const override;

private:
	std::vector<ProductListBoxItem> mItems;
};
