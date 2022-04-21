#pragma once
#include <random>

class Random
{
public:
	Random() = delete;
	Random(Random&) = delete;
	Random(Random&&) = delete;

	static double Double() { return Double(0, 1); }
	static double Double(double min, double max) { return dist_(rng_) * (max - min) + min; }

private:
	static std::random_device dev_;
	static std::mt19937 rng_;
	static std::uniform_real_distribution<double> dist_;
};


