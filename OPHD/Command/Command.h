#pragma once
#include <NAS2D/Math/Vector.h>


class Command
{
public:
	virtual ~Command() = default;
	virtual void execute() = 0;
};
