#pragma once

#include "NAS2D/NAS2D.h"

#include <string>

#include "../UI/UI.h"


const int HANDLE_MARGIN = 1;
const int HANDLE_SIZE = 8;

const int MINIMUM_WINDOW_SIZE = 100;
const int MAXIMUM_WINDOW_SIZE = 750;


void createMixer();

void createRenderer();

void initNas2d(const std::string& argv_0, const std::string& startPath, const std::string& config_path);

void updateWindowHandlePositions(Control* c, NAS2D::Rectangle_2d& top_left, NAS2D::Rectangle_2d& top_right, NAS2D::Rectangle_2d& bottom_left, NAS2D::Rectangle_2d& bottom_right);

void updateRectPosition(NAS2D::Rectangle_2d& p, int dX, int dY);

void validateVideoResolution();
