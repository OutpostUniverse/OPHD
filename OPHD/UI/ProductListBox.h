#pragma once

#include "Core/ListBoxBase.h"


namespace NAS2D {
	class Font;
}

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

		std::size_t count = 0; /**< Count of the product. */
		float usage = 0.0f; /**< Usage of available capacity. */
	};

	ProductListBox();

	void productPool(ProductPool&);

	void update() override;
private:
	const NAS2D::Font* MAIN_FONT = nullptr;
	const NAS2D::Font* MAIN_FONT_BOLD = nullptr;
};
