#include "StringTable.h"

#include <NAS2D/Resource/Font.h>
#include <NAS2D/Renderer/Renderer.h>

#include <libControls/Control.h>

#include <stdexcept>
#include <algorithm>


const NAS2D::Color StringTable::Cell::ColorEmpty = NAS2D::Color::NoAlpha;


StringTable::Cell& StringTable::operator[](const CellCoordinate& coordinate)
{
	return mCells[getCellIndex(coordinate)];
}

StringTable::StringTable(std::size_t columns, std::size_t rows) :
	mColumnCount(columns),
	mRowCount(rows)
{
	mCells.resize(columns * rows);

	mDefaultFont = &Control::getDefaultFont();
	mDefaultTitleFont = &Control::getDefaultFontBold();
}

void StringTable::draw(NAS2D::Renderer& renderer) const
{
	for (std::size_t i = 0; i < mCells.size(); ++i)
	{
		const auto& cell = mCells.at(i);

		NAS2D::Color textColor = cell.textColor != Cell::ColorEmpty ? cell.textColor : mDefaultTextColor;

		renderer.drawText(*getCellFont(i), cell.text, position() + cell.textOffset, textColor);
	}
}

void StringTable::setDefaultFont(NAS2D::Font& font)
{
	mDefaultFont = &font;
}

void StringTable::setDefaultTitleFont(const NAS2D::Font* font)
{
	mDefaultTitleFont = font;
}

void StringTable::setDefaultTextColor(NAS2D::Color color)
{
	mDefaultTextColor = color;
}

const NAS2D::Font* StringTable::GetDefaultFont() const
{
	return mDefaultFont;
}

const NAS2D::Font* StringTable::GetDefaultTitleFont() const
{
	return mDefaultTitleFont;
}

NAS2D::Color StringTable::GetDefaultFontColor() const
{
	return mDefaultTextColor;
}

void StringTable::setHorizontalPadding(int padding)
{
	mHorizontalPadding = padding;
}

void StringTable::setVerticalPadding(int padding)
{
	mVerticalPadding = padding;
}

void StringTable::setColumnText(std::size_t column, const std::vector<std::string>& rows)
{
	checkCellIndex({column, rows.size() - 1});

	for (std::size_t row = 0; row < rows.size(); ++row)
	{
		mCells[getCellIndex({column, row})].text = rows[row];
	}
}

void StringTable::setRowText(std::size_t row, const std::vector<std::string>& columns)
{
	checkCellIndex({columns.size() - 1, row});

	for (std::size_t column = 0; column < columns.size(); ++column)
	{
		mCells[getCellIndex({column, row})].text = columns[column];
	}
}

void StringTable::setColumnJustification(std::size_t column, Justification justification)
{
	for (std::size_t row = 0; row < mRowCount; ++row)
	{
		mCells[getCellIndex({column, row})].justification = justification;
	}
}

void StringTable::setColumnFont(std::size_t column, const NAS2D::Font* const font)
{
	for (std::size_t row = 0; row < mRowCount; ++row)
	{
		mCells[getCellIndex({column, row})].font = font;
	}
}

void StringTable::setRowFont(std::size_t row, const NAS2D::Font* const font)
{
	for (std::size_t column = 0; column < mColumnCount; ++column)
	{
		mCells[getCellIndex({column, row})].font = font;
	}
}

void StringTable::computeRelativeCellPositions()
{
	auto columnWidths = computeColumnWidths();
	auto rowHeights = computeRowHeights();

	int columnOffset = 0;
	for (std::size_t column = 0; column < mColumnCount; ++column)
	{
		int rowOffset = 0;
		for (std::size_t row = 0; row < mRowCount; ++row)
		{
			auto cellIndex = getCellIndex(CellCoordinate{column, row});
			mCells[cellIndex].textOffset = {columnOffset, rowOffset};
			accountForCellJustification(cellIndex, columnWidths[column]);

			rowOffset += rowHeights[row] + mVerticalPadding;
		}

		columnOffset += columnWidths[column] + mHorizontalPadding;
	}

	if (mCells.size() == 0)
	{
		mRect.size = {0, 0};
	}
	else
	{
		mRect.size = mCells.back().textOffset + NAS2D::Vector{columnWidths.back(), rowHeights.back()};
	}
}

void StringTable::accountForCellJustification(std::size_t index, int columnWidth)
{
	auto& cell = mCells[index];

	switch (cell.justification)
	{
	case (Justification::Left):
		return; // No modification required for left justification
	case (Justification::Right):
		cell.textOffset.x += columnWidth - getCellFont(index)->width(cell.text);
		return;
	case (Justification::Center):
		cell.textOffset.x += (columnWidth - getCellFont(index)->width(cell.text)) / 2;
		return;
	}
}

std::vector<int> StringTable::computeColumnWidths() const
{
	std::vector<int> columnWidths;

	for (std::size_t column = 0; column < mColumnCount; ++column)
	{
		int columnWidth = 0;

		for (std::size_t row = 0; row < mRowCount; ++row)
		{
			auto index = getCellIndex(CellCoordinate{column, row});
			columnWidth = std::max(columnWidth, getCellFont(index)->width(mCells[index].text));
		}

		columnWidths.push_back(columnWidth);
	}

	return columnWidths;
}

std::vector<int> StringTable::computeRowHeights() const
{
	std::vector<int> rowHeights;

	for (std::size_t row = 0; row < mRowCount; ++row)
	{
		int rowHeight = 0;

		for (std::size_t column = 0; column < mColumnCount; ++column)
		{
			auto index = getCellIndex(CellCoordinate{column, row});
			rowHeight = std::max(rowHeight, getCellFont(index)->height());
		}

		rowHeights.push_back(rowHeight);
	}

	return rowHeights;
}

std::size_t StringTable::getCellIndex(const CellCoordinate& cellCoordinate) const
{
	checkCellIndex(cellCoordinate);

	return mColumnCount * cellCoordinate.y + cellCoordinate.x;
}

StringTable::CellCoordinate StringTable::getCellCoordinate(std::size_t index) const
{
	return CellCoordinate{index % mColumnCount, index / mColumnCount};
}

void StringTable::checkCellIndex(const CellCoordinate& cellCoordinate) const
{
	if (cellCoordinate.x >= mColumnCount)
	{
		throw std::runtime_error("Index is outside column bounds");
	}

	if (cellCoordinate.y >= mRowCount)
	{
		throw std::runtime_error("Index is outside row bounds");
	}
}

const NAS2D::Font* StringTable::getCellFont(std::size_t index) const
{
	const NAS2D::Font* font = mCells[index].font;

	if (font == nullptr) {
		// If a different title font is not desired, it is set to nullptr
		if (mDefaultTitleFont == nullptr) {
			font = mDefaultFont;
		}
		else {
			font = isFirstColumn(index) ? mDefaultTitleFont : mDefaultFont;
		}
	}

	return font;
}

bool StringTable::isFirstColumn(std::size_t index) const
{
	return index % mColumnCount == 0;
}
