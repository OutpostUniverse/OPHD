#include "EnumIntegrityLevel.h"

#include <array>
#include <string>
#include <stdexcept>


namespace
{
	struct IntegrityTransition
	{
		int value;
		IntegrityLevel integrityLevel;
	};


	constexpr std::array<IntegrityTransition, 6> IntegrityTransitions =
	{{
		{100, IntegrityLevel::Mint},
		{80, IntegrityLevel::Good},
		{35, IntegrityLevel::Worn},
		{20, IntegrityLevel::Decayed},
		{1, IntegrityLevel::Unstable},
		{0, IntegrityLevel::Destroyed},
	}};
}


IntegrityLevel integrityLevel(int integrity)
{
	for (const auto& integrityTransition : IntegrityTransitions)
	{
		if (integrity >= integrityTransition.value) { return integrityTransition.integrityLevel; }
	}
	throw std::runtime_error("Bad integrity value: " + std::to_string(integrity));
}
