#include "ProductListBox.h"

#include "ProgressBar.h"

#include "../Cache.h"
#include "../Constants/UiConstants.h"
#include "../ProductCatalogue.h"
#include "../ProductPool.h"

#include <NAS2D/Utility.h>
#include <NAS2D/Renderer/Renderer.h>


using namespace NAS2D;


ProductListBox::ProductListBox() :
	ListBoxBase{
		fontCache.load(constants::FONT_PRIMARY, 12),
		fontCache.load(constants::FONT_PRIMARY_BOLD, 12)
	}
{
	itemHeight(30);
}


/**
 * Fills the ProductListBox with Products in a ProductPool object.
 */
void ProductListBox::productPool(ProductPool& pool)
{
	clear();

	const auto productTypeCount = static_cast<std::size_t>(ProductType::PRODUCT_COUNT);

	for (std::size_t productTypeIndex = 0; productTypeIndex < productTypeCount; ++productTypeIndex)
	{
		const auto productType = static_cast<ProductType>(productTypeIndex);
		const auto productCount = pool.count(productType);

		if (productCount > 0)
		{
			ProductListBoxItem* item = new ProductListBoxItem();
			item->text = ProductCatalogue::get(productType).Name;
			item->count = productCount;
			item->capacityUsed = productCount * pool.productStorageRequirement(productType);
			item->capacityTotal = pool.capacity();
			mItems.push_back(item);
		}
	}

	updateScrollLayout();
}


/**
 * Draws the FactoryListBox
 */
void ProductListBox::update()
{
	if (!visible()) { return; }
	ListBoxBase::update();

	auto& renderer = Utility<Renderer>::get();

	renderer.clipRect(mRect);

	constexpr Color highlightColor{0, 185, 0, 75};

	const auto itemSize = NAS2D::Vector{itemWidth(), itemHeight()}.to<int>();
	const auto firstStop = itemSize.x / 3;
	const auto secondStop = itemSize.x * 2 / 3;
	const auto offset = static_cast<int>(drawOffset());

	for (std::size_t i = 0; i < mItems.size(); ++i)
	{
		const auto& item = *static_cast<ProductListBoxItem*>(mItems[i]);
		const auto drawPosition = NAS2D::Point{positionX(), positionY() + (static_cast<int>(i) * itemSize.y) - offset};
		const auto highlight = i == selectedIndex();

		// Draw highlight rect so as not to tint/hue colors of everything else
		if (highlight) { renderer.drawBoxFilled(NAS2D::Rectangle{drawPosition, itemSize}, highlightColor); }

		// Draw item borders and column breaks
		renderer.drawBox(NAS2D::Rectangle<int>{drawPosition + Vector{2, 2}, {itemSize.x - 4, itemSize.y - 4}}, constants::PrimaryColor);
		renderer.drawLine(drawPosition + NAS2D::Vector{firstStop, 2}, drawPosition + NAS2D::Vector{firstStop, itemSize.y - 2}, constants::PrimaryColor);
		renderer.drawLine(drawPosition + NAS2D::Vector{secondStop, 2}, drawPosition + NAS2D::Vector{secondStop, itemSize.y - 2}, constants::PrimaryColor);

		// Draw item column contents
		renderer.drawText(mFontBold, item.text, drawPosition + NAS2D::Vector{5, 15 - mFontBold.height() / 2}, constants::PrimaryColor);
		renderer.drawText(mFont, "Quantity: " + std::to_string(item.count), drawPosition + NAS2D::Vector{firstStop + 5, 15 - mFontBold.height() / 2}, constants::PrimaryColor);
		drawProgressBar(
			item.capacityUsed,
			item.capacityTotal,
			{drawPosition + NAS2D::Vector{secondStop + 5, 10}, {firstStop - 10, 10}},
			2
		);
	}

	renderer.clipRectClear();
}
