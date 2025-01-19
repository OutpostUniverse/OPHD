#include "TrendLabel.h"


std::size_t trendIndex(int changeValue)
{
	return (changeValue < 0) ? 0 :
		(changeValue > 0) ? 2 : 1;
}


std::string formatDiff(int changeValue)
{
	return ((changeValue > 0) ? "+" : "") + std::to_string(changeValue);
}
