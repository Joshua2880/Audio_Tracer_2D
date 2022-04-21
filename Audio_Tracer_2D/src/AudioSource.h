#pragma once

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
		double delta_theta = 2 * M_PI / num_rays;
		for (size_t i = 0; i < num_rays; ++i)
		{
			cast({pos_, Vec2::Rand(i * delta_theta, (i + 1) * delta_theta), 0, 1});
		}
	}

private:
	Vec2 pos_;
};
