#pragma once

#include <array>
#include <vector>

#include "AABB.h"
#include "Globals.h"
#include "Ray.h"
#include "Utilities.h"
#include "Vec2.h"

class Object
{
public:
	Object(std::vector<Vec2> vertices,
		   std::vector<std::array<size_t, 2>> edges) : vertices_{vertices},
		                                               edges_{edges} {}

	void Draw(WndState &wnd_state, AABB space) const
	{
		for (auto &edge : edges_)
		{
			Vec2 start = (vertices_[edge[0]] - Vec2{ space.x_min, space.y_min }) / space.Width() * static_cast<double>(wnd_state.bitmap_info.bmiHeader.biWidth);
			Vec2 end = (vertices_[edge[1]] - Vec2{ space.x_min, space.y_min }) / space.Width() * static_cast<double>(wnd_state.bitmap_info.bmiHeader.biWidth);
			DrawLine(wnd_state, start, end);
		}
	}

	Ray Hit(Ray r) const
	{
		Ray closest{ {0, 0}, { 0, 0 }, std::numeric_limits<double>::infinity(), 0 };
		for (size_t i = 0; i < edges_.size(); ++i)
		{
			Vec2 edge_start = vertices_[edges_[i][0]];
			Vec2 edge_end = vertices_[edges_[i][1]];
			double dt = ((r.position.x - edge_start.x) * (edge_start.y - edge_end.y) - (r.position.y - edge_start.y) * (edge_start.x - edge_end.x)) / (( - r.direction.x) * (edge_start.y - edge_end.y) - ( - r.direction.y) * (edge_start.x - edge_end.x)) / SPEED_OF_SOUND;
			double para = ((r.position.x - edge_start.x) * (- r.direction.y) - (r.position.y - edge_start.y) * (- r.direction.x)) / (( - r.direction.x) * (edge_start.y - edge_end.y) - ( - r.direction.y) * (edge_start.x - edge_end.x));
			if (para < 0 | para > 1 | dt < std::numeric_limits<float>::epsilon() | dt > closest.time - r.time)
			{
				continue;
			}
			Vec2 normal = Normal(i);
			closest = { r.position + dt * r.direction, r.direction - 2 * Dot(r.direction, normal) * normal, r.time + dt, r.amplitude};
		}
		return closest;
	}

	Vec2 Normal(size_t edge) const
	{
		Vec2 edge_start = vertices_[edges_[edge][0]];
		Vec2 edge_end = vertices_[edges_[edge][1]];
		Vec2 delta = edge_end - edge_start;
		return Vec2{ -delta.y, delta.x }.Normalize();
	}

private:
	std::vector<Vec2> vertices_;
	std::vector<std::array<size_t, 2>> edges_;
};
