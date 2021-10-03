#pragma once

#include <NAS2D/Math/Point.h>
#include <NAS2D/Renderer/Color.h>

#include <string>


void drawLabelAndValue(NAS2D::Point<int> position, const std::string& title, const std::string& text, NAS2D::Color color = NAS2D::Color::White);
void drawLabelAndValueLeftJustify(NAS2D::Point<int> position, int labelWidth, const std::string& title, const std::string& text, NAS2D::Color color = NAS2D::Color::White);
void drawLabelAndValueRightJustify(NAS2D::Point<int> position, int labelWidth, const std::string& title, const std::string& text, NAS2D::Color color = NAS2D::Color::White);
