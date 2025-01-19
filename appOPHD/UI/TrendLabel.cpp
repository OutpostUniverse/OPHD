#include "TrendLabel.h"


std::string formatDiff(int changeValue)
{
	return ((changeValue > 0) ? "+" : "") + std::to_string(changeValue);
}
