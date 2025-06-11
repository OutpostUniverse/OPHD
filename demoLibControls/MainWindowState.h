#pragma once


#include <NAS2D/State.h>


class MainWindowState : public NAS2D::State
{
public:
	State* update() override;
};
