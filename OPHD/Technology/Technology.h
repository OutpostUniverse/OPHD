#pragma once

#include <string>
#include <vector>

struct Effect;

struct Technology
{
	std::string name;
	std::string description;
	
	unsigned int id{0};
	unsigned int labType{0};
	unsigned int cost{0};
	std::vector<unsigned int> requiredTechnologies;
	std::vector<Effect> effects;
};
