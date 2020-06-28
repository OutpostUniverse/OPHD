#pragma once

#include <NAS2D/Renderer/Point.h>
#include <NAS2D/Renderer/Color.h>
#include <string>


void drawLabelAndValue(NAS2D::Point<int> position, const std::string& title, const std::string& text, int valuePadding = 0, NAS2D::Color color = NAS2D::Color::White);
void drawLabelAndValueLeftJustify(NAS2D::Point<int> position, const std::string& title, const std::string& text, int labelWidth, NAS2D::Color color = NAS2D::Color::White);
