#pragma once
#include "Vec2.h"

struct AABB
{
	friend AABB operator*(AABB aabb, Vec2 v) { return { aabb.x_min * v.x, aabb.y_min * v.y, aabb.x_max * v.x, aabb.y_max * v.y }; }
	friend AABB operator*(Vec2 v, AABB aabb) { return aabb * v; }
	friend AABB operator*(AABB aabb, double s) { return { aabb.x_min * s, aabb.y_min * s, aabb.x_max * s, aabb.y_max * s }; }
	friend AABB operator*(double s, AABB aabb) { return aabb * s; }

	AABB& operator*=(double s) { return *this = *this * s; }
	AABB& operator*= (Vec2 v) { return *this = *this * v; }

	double Width() const { return x_max - x_min; }
	double Height() const { return y_max - y_min; }

	bool Contains(Vec2 v) { return v.x >= x_min && v.x <= x_max && v.y >= y_min && v.y <= y_max; }

	double x_min, y_min;
	double x_max, y_max;
};
