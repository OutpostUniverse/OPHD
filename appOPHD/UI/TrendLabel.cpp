#include "TrendLabel.h"


std::string formatDiff(int diff)
{
	return ((diff > 0) ? "+" : "") + std::to_string(diff);
}
