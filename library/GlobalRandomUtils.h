#pragma once
#include <random>

namespace Utils
{
	inline std::default_random_engine &GetRandomEngine()
	{
		static std::random_device s_oRandomDevice;
		static std::default_random_engine s_oDefaultRandomEngine(s_oRandomDevice());

		return s_oDefaultRandomEngine;
	}

	template <typename T>
	T Random(T i_nMinimal, T i_nMaximal)
	{
		return std::uniform_int_distribution<T>(i_nMinimal, i_nMaximal)(GetRandomEngine());
	}
}