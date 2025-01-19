#pragma once

#include <string>

enum class MoraleIndexs;


const std::string& moraleString(std::size_t index);
const std::string& moraleString(MoraleIndexs morale);
std::size_t moraleStringTableCount();
