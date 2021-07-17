#pragma once

#include "Core/ListBoxBase.h"


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
		float usage = 0.0f; /**< Usage of available capacity. */
	};

	ProductListBox();

	void productPool(ProductPool&);

	void update() override;
private:
	const NAS2D::Font& mFont;
	const NAS2D::Font& mFontBold;
};
