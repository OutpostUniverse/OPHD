#include "StringTable.h"
#include "../FontManager.h"
#include "../Constants/UiConstants.h"
#include <NAS2D/Utility.h>
#include <stdexcept>

const NAS2D::Color StringTable::Cell::ColorEmpty = NAS2D::Color::NoAlpha;

StringTable::StringTable(std::size_t columns, std::size_t rows) : columns(columns), rows(rows)
{
	cells.resize(columns * rows);

	defaultFont = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY, constants::FONT_PRIMARY_NORMAL);
	defaultTitleFont = NAS2D::Utility<FontManager>::get().font(constants::FONT_PRIMARY_BOLD, constants::FONT_PRIMARY_NORMAL);
}

void StringTable::draw(NAS2D::Renderer& renderer) const
{
	for (std::size_t i = 0; i < cells.size(); ++i)
	{
		const Cell& cell = cells.at(i);

		NAS2D::Color textColor = cell.textColor != Cell::ColorEmpty ? cell.textColor : defaultTextColor;
		NAS2D::Point<float> drawPosition = { position.x() + cell.textRelativePosition.x(), position.y() + cell.textRelativePosition.y() };

		renderer.drawText(*getCellFont(i), cell.text, drawPosition, textColor);
	}
}

void StringTable::setPosition(NAS2D::Point<float> tablePosition)
{
	this->position = tablePosition;
}

NAS2D::Point<float> StringTable::getPosition() const
{
	return position;
}

void StringTable::setDefaultFont(NAS2D::Font* font)
{
	if (font == nullptr)
	{
		throw std::runtime_error("Default font may not be a nullptr");
	}

	defaultFont = font;
}

void StringTable::setDefaultTitleFont(NAS2D::Font* font)
{
	defaultTitleFont = font;
}

void StringTable::setDefaultTextColor(NAS2D::Color color)
{
	defaultTextColor = color;
}

void StringTable::setHorizontalPadding(float padding)
{
	horizontalPadding = padding;
}

void StringTable::setVerticalPadding(float padding)
{
	verticalPadding = padding;
}

void StringTable::setCellText(std::size_t column, std::size_t row, std::string text)
{
	setCellText(CellCoordinate(column, row), text);
}

void StringTable::setCellText(const CellCoordinate& cellCoordinate, std::string text)
{
	cells[getCellIndex(cellCoordinate)].text = text;
}

void StringTable::setCellFont(const CellCoordinate& cellCoordinate, NAS2D::Font* font)
{
	cells[getCellIndex(cellCoordinate)].font = font;
}

void StringTable::setCellJustification(const CellCoordinate& cellCoordinate, Justification justification)
{
	cells[getCellIndex(cellCoordinate)].justification = justification;
}

void StringTable::setColumnJustification(std::size_t column, Justification justification)
{
	for (std::size_t row = 0; row < rows; ++row)
	{
		setCellJustification(CellCoordinate(column, row), justification);
	}
}

void StringTable::setCellTextColor(const CellCoordinate& cellCoordinate, NAS2D::Color color)
{
	cells[getCellIndex(cellCoordinate)].textColor = color;
}

void StringTable::computeRelativeCellPositions()
{
	// Must be at least 1 row and 1 column to compute position values
	if (columns == 0 || rows == 0)
	{
		return;
	}

	auto columnWidths = computeColumnWidths();
	auto rowHeights = computeRowHeights();

	float columnOffset = 0;
	for (std::size_t column = 0; column < columns; ++column)
	{
		float rowOffset = 0;
		for (std::size_t row = 0; row < rows; ++row)
		{
			auto cellIndex = getCellIndex(CellCoordinate(column, row));
			cells[cellIndex].textRelativePosition = { columnOffset, rowOffset };
			accountForCellJustification(cellIndex, columnWidths[column]);

			rowOffset += rowHeights[row] + verticalPadding;
		}

		columnOffset += columnWidths[column] + horizontalPadding;
	}
}

void StringTable::accountForCellJustification(std::size_t index, float columnWidth)
{
	Cell& cell = cells[index];

	switch (cell.justification)
	{
	case (Justification::Left):
		return; // No modification required for left justifited
	case (Justification::Right):
		cell.textRelativePosition.x() += columnWidth - getCellFont(index)->width(cell.text);
		return;
	default:
		return;
	}
}

std::vector<float> StringTable::computeColumnWidths() const
{
	std::vector<float> columnWidths;

	for (std::size_t column = 0; column < columns; ++column)
	{
		float columnWidth = 0;

		for (std::size_t row = 0; row < rows; ++row)
		{
			auto index = getCellIndex(CellCoordinate(column, row));
			auto cellWidth = static_cast<float>(getCellFont(index)->width(cells[index].text));

			if (cellWidth > columnWidth)
			{
				columnWidth = cellWidth;
			}
		}

		columnWidths.push_back(columnWidth);
	}

	return columnWidths;
}

std::vector<float> StringTable::computeRowHeights() const
{
	std::vector<float> rowHeights;

	for (std::size_t row = 0; row < rows; ++row)
	{
		float rowHeight = 0;

		for (std::size_t column = 0; column < columns; ++column)
		{
			auto index = getCellIndex(CellCoordinate(column, row));
			float cellHeight = static_cast<float>(getCellFont(index)->height());

			if (cellHeight > rowHeight)
			{
				rowHeight = cellHeight;
			}
		}

		rowHeights.push_back(rowHeight);
	}

	return rowHeights;
}

std::size_t StringTable::getCellIndex(const CellCoordinate& cellCoordinate) const
{
	checkCellIndex(cellCoordinate);

	return columns * cellCoordinate.y() + cellCoordinate.x();
}

StringTable::CellCoordinate StringTable::getCellCoordinate(std::size_t index) const
{
	return CellCoordinate{ index % columns, index / columns };
}

void StringTable::checkCellIndex(const CellCoordinate& cellCoordinate) const
{
	if (cellCoordinate.x() >= columns)
	{
		throw std::runtime_error("Index is outside column bounds");
	}

	if (cellCoordinate.y() >= rows)
	{
		throw std::runtime_error("Index is outside row bounds");
	}
}

NAS2D::Font* StringTable::getCellFont(std::size_t index) const
{
	NAS2D::Font* font = cells[index].font;// = index % columns == 0 ? defaultTitleFont : defaultFont;

	if (font == nullptr) {
		font = isFirstColumn(index) ? defaultTitleFont : defaultFont;
	}

	return font;
}

bool StringTable::isFirstColumn(std::size_t index) const
{
	return index % columns == 0;
}
