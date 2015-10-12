#pragma once

#include "Structure.h"

class Factory : public Structure
{

public:
	Factory(const std::string& name, const std::string& sprite_path);
	virtual ~Factory();

	virtual void updateProduction();

protected:

private:

};