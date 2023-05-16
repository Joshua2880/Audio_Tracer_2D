#pragma once

#include <cmath>
#include <complex>
#include <corecrt_math_defines.h>
#include <mutex>
#include <vector>

#include "Globals.h"

class IR
{
public:
	IR(size_t sample_length) : m_{}, clip_ { std::vector<float>(sample_length) } {}
	IR(IR const &o) : m_{}, clip_ { std::vector<float>(o.clip_.size()) } {}

	void Add(double time, float amplitude)
	{
		double offset = time * SAMPLE_RATE;
		std::unique_lock lock{ m_ };
		for (size_t i = 0; i < BINS; ++i)
		{
			double pos = M_PI * static_cast<double>(i) - offset;
			clip_[i] += amplitude * static_cast<float>(sin(pos) / pos);
		}
	}

	std::complex<float> BinMagnitude(size_t bin) const
	{
		std::complex<float> step = std::exp(-std::complex<float>{0, 1} * 2.0f * static_cast<float>(M_PI) * static_cast<float>(bin) / static_cast<float>(BINS));
		std::complex<float> phase = 1;
		std::complex<float> acc = 0;
		for (size_t i = 0; i < BINS; ++i)
		{
			acc += clip_[i] * phase;
			phase *= step;
		}
		return acc / sqrtf(BINS);
	}

private:
	std::mutex m_;
	std::vector<float> clip_;
};
