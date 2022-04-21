#pragma once

#include <cmath>
#include <corecrt_math_defines.h>

#include "Random.h"

struct Vec2
{
	Vec2 operator-() const { return { -x, -y }; }

	double Magnitude() const { return sqrt(x * x + y * y); }

	Vec2 Normalize() const { return *this / Magnitude(); }

	friend double Dot(Vec2 v1, Vec2 v2) { return v1.x * v2.x + v1.y * v2.y; }

	friend Vec2 operator+(Vec2 v1, Vec2 v2) { return { v1.x + v2.x, v1.y + v2.y }; }
	friend Vec2 operator-(Vec2 v1, Vec2 v2) { return { v1.x - v2.x, v1.y - v2.y }; }
	friend Vec2 operator*(double s, Vec2 v) { return { s * v.x, s * v.y }; }
	friend Vec2 operator*(Vec2 v, double s) { return s * v; }
	friend Vec2 operator/(Vec2 v, double s) { return 1 / s * v; }

	static Vec2 Rand()
	{
		Rand(0, 2 * M_PI);
	}

	static Vec2 Rand(double start_angle, double end_angle)
	{
		double theta = Random::Double(start_angle, end_angle);
		return { cos(theta), sin(theta) };
	}

	double x{}, y{};
};
