#pragma once

#include "Vec2.h"

struct Ray
{
	Vec2 position;
	Vec2 direction;
	double time;
	float amplitude;
};