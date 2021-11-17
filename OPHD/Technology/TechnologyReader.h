#pragma once

#include "Technology.h"

#include <map>
#include <string>

class TechnologyReader
{
public:
	TechnologyReader() = delete;
	TechnologyReader(const std::string& techFile);

	std::map<int, Technology> technologies;

private:
	
};