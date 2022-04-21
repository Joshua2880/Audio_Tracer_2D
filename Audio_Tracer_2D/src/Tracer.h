#pragma once

#include "AABB.h"
#include "AudioSource.h"
#include "IRArray.h"
#include "Object.h"
#include "Ray.h"
#include "Vec2.h"

class Tracer
{
public:
	Tracer(AABB aabb, Vec2 audio_source_pos) : ir_array_(new IRArray{aabb}),
	                                           audio_source_(new AudioSource{audio_source_pos}) {}

	void AddObject(Object &object)
	{
		objects_.emplace_back(object);
	}

	void Calculate()
	{
		audio_source_->Project([this](Ray r) { Cast(r); });
	}

	void Draw(WndState &wnd_state, AABB space) const
	{
		ir_array_->Draw(wnd_state, space);
		for(auto &object : objects_)
		{
			object.Draw(wnd_state, space);
		}
	}

	double getGridSpacing() const
	{
		return ir_array_->getGridSpacing();
	}

private:
	void Cast(Ray r)
	{
		if (r.time > BINS / SAMPLE_RATE) return;
		Ray closest = ir_array_->Hit(r);
		for(auto &object : objects_)
		{
			Ray temp = object.Hit(r);
			if (temp.time < closest.time) closest = temp;
		}
		Cast(closest);
	}

	IRArray *ir_array_{};
	AudioSource *audio_source_{};
	std::vector<Object> objects_{};
};
