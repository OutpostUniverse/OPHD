#include "StringTable.h"
#include "../FontManager.h"
#include "../Constants/UiConstants.h"
#include <NAS2D/Utility.h>
#include <stdexcept>
#include <algorithm>

const NAS2D::Color StringTable::Cell::ColorEmpty = NAS2D::Color::NoAlpha;

StringTable::Cell& StringTable::operator[](const CellCoordinate& coordinate)
{
	return mCells[getCellIndex(coordinate)];
}

StringTable::Cell& StringTable::at(std::size_t column, std::size_t row)
{
	return mCells[getCellIndex(CellCoordinate(column, row))];
}

StringTable::StringTable(std::size_t columns, std::size_t rows) : mColumnCount(columns), mRowCount(rows)
{
	mCells.resize(columns * rows);

	mDefaultFont = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	mDefaultTitleFont = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
}

void StringTable::draw(NAS2D::Renderer& renderer) const
{
	for (std::size_t i = 0; i < mCells.size(); ++i)
	{
		const auto& cell = mCells.at(i);

		NAS2D::Color textColor = cell.textColor != Cell::ColorEmpty ? cell.textColor : mDefaultTextColor;

		renderer.drawText(*getCellFont(i), cell.text, mPosition + cell.textRelativePosition, textColor);
	}
}

void StringTable::position(NAS2D::Point<float> position)
{
	this->mPosition = position;
}

NAS2D::Point<float> StringTable::position() const
{
	return mPosition;
}

void StringTable::setDefaultFont(NAS2D::Font& font)
{
	mDefaultFont = &font;
}

void StringTable::setDefaultTitleFont(NAS2D::Font* font)
{
	mDefaultTitleFont = font;
}

void StringTable::setDefaultTextColor(NAS2D::Color color)
{
	mDefaultTextColor = color;
}

void StringTable::setHorizontalPadding(float padding)
{
	mHorizontalPadding = padding;
}

void StringTable::setVerticalPadding(float padding)
{
	mVerticalPadding = padding;
}

void StringTable::setColumnText(std::size_t column, const std::vector<NAS2D::StringValue>& rows)
{
	checkCellIndex({ column, rows.size() - 1});

	for (std::size_t row = 0; row < rows.size(); ++row)
	{
		mCells[getCellIndex({ column, row })].text = rows[row].to<std::string>();
	}
}

void StringTable::setRowText(std::size_t row, const std::vector<NAS2D::StringValue>& columns)
{
	checkCellIndex({ columns.size() - 1, row });

	for (std::size_t column = 0; column < columns.size(); ++column)
	{
		mCells[getCellIndex({ column, row })].text = columns[column].to<std::string>();
	}
}

void StringTable::setColumnJustification(std::size_t column, Justification justification)
{
	for (std::size_t row = 0; row < mRowCount; ++row)
	{
		this->at(column, row).justification = justification;
	}
}

void StringTable::computeRelativeCellPositions()
{
	auto columnWidths = computeColumnWidths();
	auto rowHeights = computeRowHeights();

	float columnOffset = 0;
	for (std::size_t column = 0; column < mColumnCount; ++column)
	{
		float rowOffset = 0;
		for (std::size_t row = 0; row < mRowCount; ++row)
		{
			auto cellIndex = getCellIndex(CellCoordinate(column, row));
			mCells[cellIndex].textRelativePosition = { columnOffset, rowOffset };
			accountForCellJustification(cellIndex, columnWidths[column]);

			rowOffset += rowHeights[row] + mVerticalPadding;
		}

		columnOffset += columnWidths[column] + mHorizontalPadding;
	}
}

void StringTable::accountForCellJustification(std::size_t index, float columnWidth)
{
	auto& cell = mCells[index];

	switch (cell.justification)
	{
	case (Justification::Left):
		return; // No modification required for left justifited
	case (Justification::Right):
		cell.textRelativePosition.x += columnWidth - getCellFont(index)->width(cell.text);
		return;
	default:
		return;
	}
}

std::vector<float> StringTable::computeColumnWidths() const
{
	std::vector<float> columnWidths;

	for (std::size_t column = 0; column < mColumnCount; ++column)
	{
		float columnWidth = 0;

		for (std::size_t row = 0; row < mRowCount; ++row)
		{
			auto index = getCellIndex(CellCoordinate(column, row));
			columnWidth = std::max(columnWidth, static_cast<float>(getCellFont(index)->width(mCells[index].text)));
		}

		columnWidths.push_back(columnWidth);
	}

	return columnWidths;
}

std::vector<float> StringTable::computeRowHeights() const
{
	std::vector<float> rowHeights;

	for (std::size_t row = 0; row < mRowCount; ++row)
	{
		float rowHeight = 0;

		for (std::size_t column = 0; column < mColumnCount; ++column)
		{
			auto index = getCellIndex(CellCoordinate(column, row));
			rowHeight = std::max(rowHeight, static_cast<float>(getCellFont(index)->height()));
		}

		rowHeights.push_back(rowHeight);
	}

	return rowHeights;
}

std::size_t StringTable::getCellIndex(const CellCoordinate& cellCoordinate) const
{
	checkCellIndex(cellCoordinate);

	return mColumnCount * cellCoordinate.y() + cellCoordinate.x();
}

StringTable::CellCoordinate StringTable::getCellCoordinate(std::size_t index) const
{
	return CellCoordinate{ index % mColumnCount, index / mColumnCount };
}

void StringTable::checkCellIndex(const CellCoordinate& cellCoordinate) const
{
	if (cellCoordinate.x() >= mColumnCount)
	{
		throw std::runtime_error("Index is outside column bounds");
	}

	if (cellCoordinate.y() >= mRowCount)
	{
		throw std::runtime_error("Index is outside row bounds");
	}
}

NAS2D::Font* StringTable::getCellFont(std::size_t index) const
{
	NAS2D::Font* font = mCells[index].font;

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
