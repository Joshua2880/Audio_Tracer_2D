#pragma once

#include <windows.h>
#include <cmath>

#include "WndState.h"
#include "Vec2.h"

inline void DrawLine(void* pixels, size_t width, size_t height, Vec2 start, Vec2 end)
{
	Vec2 delta = end - start;
	double slope = delta.y / delta.x;
	size_t num_p = static_cast<size_t>(ceil(abs(abs(slope) <= 1 ? delta.x : delta.y)));
	Vec2 step = delta / static_cast<double>(num_p);
	for (size_t i = 0; i < num_p; ++i)
	{
		Vec2 p = start + i * step;
		if (static_cast<size_t>(p.x) < 0 || static_cast<size_t>(p.x) >= width ||
			static_cast<size_t>(p.y) < 0 || static_cast<size_t>(p.y) >= height)
		{
			continue;
		}
		static_cast<uint32_t*>(pixels)[static_cast<size_t>(p.y) * width + static_cast<size_t>(p.x)] = 0x00FFFFFF;
	}
}

inline void DrawLine(WndState &wnd_state, Vec2 start, Vec2 end)
{
	DrawLine(wnd_state.bitmap_memory, 
			 wnd_state.bitmap_info.bmiHeader.biWidth, 
			 wnd_state.bitmap_info.bmiHeader.biHeight, 
			 start,
			 end);
}