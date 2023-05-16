#pragma once

#include <thread>
#include <functional>

#include "Ray.h"
#include "Vec2.h"

class AudioSource
{
public:	
	AudioSource(Vec2 pos) : pos_{pos} {}

	template<typename F>
	void Project(F const &cast) const
	{
		size_t num_rays = 100000;
		size_t num_threads = 2 * std::thread::hardware_concurrency();
		size_t block_size = num_rays / num_threads;
		size_t remainder = num_rays % num_threads;
		std::vector<std::thread> threads{};
		size_t offset = 0;
		for (size_t i = 0; i < num_threads - 1; ++i)
		{
			size_t next_offset = offset + block_size + (i < remainder);
			threads.emplace_back([=]() {ProjectHelper(cast, offset, next_offset, num_rays); });
			offset = next_offset;
		}
		ProjectHelper(cast, offset, num_rays, num_rays);
		for (size_t i = 0; i < threads.size(); ++i)
		{
			threads[i].join();
		}
	}

private:
	template<typename F>
	void ProjectHelper(F const& cast, size_t start, size_t end, size_t total) const
	{
		double delta_theta = 2 * M_PI / total;
		for (size_t i = start; i < end; ++i)
		{
			cast({ pos_, Vec2::Rand(i * delta_theta, (i + 1) * delta_theta), 0, 1 });
		}
	}

	Vec2 pos_;
};
