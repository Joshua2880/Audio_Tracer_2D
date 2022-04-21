#pragma once

class Tracer;

struct WndState
{
	BITMAPINFO bitmap_info;
	void* bitmap_memory;
	bool running;
	Tracer *tracer;
	int64_t freq_bin;
};