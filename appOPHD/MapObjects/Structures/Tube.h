#pragma once

#include "../Structure.h"


class Tube : public Structure
{
public:
	Tube(ConnectorDir dir, bool underground);

private:
	static const std::string& getAnimationName(ConnectorDir dir, bool underground);
};
