#pragma once

#include <type_traits>
#include <stdexcept>
#include <random>


class RandomNumberGenerator
{
public:
	RandomNumberGenerator() : generator(randomDevice()) {}

	template <typename T>
	requires std::is_arithmetic_v<T>
	T generate(T min, T max)
	{
		if (min > max)
		{
			throw std::runtime_error("When requesting a random number, min must be less than or equal to max.");
		}

		if constexpr (std::is_integral_v<T>)
		{
			std::uniform_int_distribution<T> distribution(min, max);
			return distribution(generator);
		}
		else if constexpr (std::is_floating_point_v<T>)
		{
			std::uniform_real_distribution<T> distribution(min, max);
			return distribution(generator);
		}
		else
		{
			throw std::runtime_error("Improper type provided for generating a random number");
		}
	}

private:
	std::random_device randomDevice;
	std::mt19937 generator;
};

inline RandomNumberGenerator randomNumber;
