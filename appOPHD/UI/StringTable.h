#pragma once

#include <libControls/Control.h>

#include <NAS2D/Renderer/Color.h>
#include <NAS2D/Math/Point.h>
#include <NAS2D/Math/Rectangle.h>
#include <NAS2D/Math/Vector.h>

#include <string>
#include <vector>
#include <cstddef>


namespace NAS2D
{
	class Font;
	class Renderer;
}


// Draw a 2 dimensional table of text. Determine cell size based on inserted text, font, and padding. Only allows one line of text per cell.
class StringTable : public Control
{
public:
	using CellCoordinate = NAS2D::Point<std::size_t>;

	enum class Justification
	{
		Left,
		Right,
		Center
	};

	struct Cell
	{
		// Set textColor to ColorEmpty to indicate cell should use default StringTable color
		static const NAS2D::Color ColorEmpty;

		// Use StringTable::mDefaultFont if not set
		const NAS2D::Font* font = nullptr;
		std::string text;
		Justification justification = Justification::Left;
		NAS2D::Color textColor = ColorEmpty;
	};

	Cell& operator[](const CellCoordinate& coordinate);

	// Set default fonts in constructor
	StringTable(std::size_t columns, std::size_t rows);

	void draw(NAS2D::Renderer& renderer) const override;

	void setDefaultFont(NAS2D::Font& font);
	void setDefaultTitleFont(const NAS2D::Font* font);
	void setDefaultTextColor(NAS2D::Color textColor);

	const NAS2D::Font* GetDefaultFont() const;
	const NAS2D::Font* GetDefaultTitleFont() const;
	NAS2D::Color GetDefaultFontColor() const;

	void setHorizontalPadding(int horizontalPadding);
	void setVerticalPadding(int verticalPadding);

	void setColumnText(std::size_t column, const std::vector<std::string>& rows);
	void setRowText(std::size_t row, const std::vector<std::string>& columns);
	void setColumnJustification(std::size_t column, Justification justification);

	void setColumnFont(std::size_t column, const NAS2D::Font* const font);
	void setRowFont(std::size_t row, const NAS2D::Font* const font);

	// Call after updating table properties to recompute cell positions
	void computeRelativeCellPositions();

protected:
	void accountForCellJustification(std::size_t index, int columnWidth);
	std::vector<int> computeColumnWidths() const;
	std::vector<int> computeRowHeights() const;

	std::size_t getCellIndex(const CellCoordinate& cellCoordinate) const;
	CellCoordinate getCellCoordinate(std::size_t index) const;
	void checkCellIndex(const CellCoordinate& cellCoordinate) const;

	const NAS2D::Font* getCellFont(std::size_t index) const;
	bool isFirstColumn(std::size_t index) const;

private:
	// Purposely hide textOffset from public access
	struct CellWithPosition : Cell
	{
		// Position relative to the StringTable's position
		NAS2D::Vector<int> textOffset;
	};

	std::vector<CellWithPosition> mCells;
	const std::size_t mColumnCount;
	const std::size_t mRowCount;
	const NAS2D::Font* mDefaultFont;
	const NAS2D::Font* mDefaultTitleFont;
	NAS2D::Color mDefaultTextColor = NAS2D::Color::White;
	int mHorizontalPadding = 5;
	int mVerticalPadding = 0;
};
