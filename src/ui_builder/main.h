#pragma once

#include "NAS2D/NAS2D.h"

#include "../UI/UI.h"
#include "../Common.h"

#include <map>
#include <vector>


using namespace NAS2D;
using namespace std;


enum ResizeHandle
{
	RESIZE_TOP_LEFT,
	RESIZE_TOP_RIGHT,
	RESIZE_BOTTOM_LEFT,
	RESIZE_BOTTOM_RIGHT,
	RESIZE_NONE
};


ResizeHandle getHandle();

void addControl(Control* c, const std::string& name);
void controlResized(Control* c);
void deleteControl(Control *c);
void repositionControl(Control* c, int relX, int relY);
void resizeControl(Control* c, int dX, int dY);
void controlTextChanged(Control* c);

bool mouseInHandles();
void setSelectedControl(Control* c);

void onKeyDown(KeyCode key, KeyModifier mod, bool repeat);
void onMouseMotion(int x, int y, int relX, int relY);
void onMouseDown(MouseButton button, int x, int y);
void onMouseUp(MouseButton button, int x, int y);
void onQuit();

void initUi();
void initEventHandlers();

void drawHandles();
void drawUi();

void updateHandles(float dX, float dY);
void addControlClicked();
