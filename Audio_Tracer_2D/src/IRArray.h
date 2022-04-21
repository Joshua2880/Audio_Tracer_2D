#pragma once

#include <cmath>
#include <cstdint>

#include "Globals.h"
#include "AABB.h"
#include "IR.h"
#include "Ray.h"
#include "Utilities.h"

class IRArray
{
public:
	IRArray(AABB bounds)
	{
		bounds_.x_min = floor(bounds.x_min / grid_spacing_) * grid_spacing_;
		bounds_.y_min = floor(bounds.y_min / grid_spacing_) * grid_spacing_;
		bounds_.x_max = ceil(bounds.x_max / grid_spacing_) * grid_spacing_;
		bounds_.y_max = ceil(bounds.y_max / grid_spacing_) * grid_spacing_;

		size_t grid_width = static_cast<int>(round((bounds_.x_max - bounds_.x_min) / grid_spacing_));
		size_t grid_height = static_cast<int>(round((bounds_.y_max - bounds_.y_min) / grid_spacing_));

		clips_ = std::vector<std::vector<IR>>(grid_width, std::vector<IR>(grid_height, BINS));
	}

	void Draw(WndState& wnd_state, AABB space) const
	{
		Vec2 ratio = {static_cast<double>(wnd_state.bitmap_info.bmiHeader.biWidth) / (space.x_max - space.x_min),
					  static_cast<double>(wnd_state.bitmap_info.bmiHeader.biHeight) / (space.y_max - space.y_min)};
		AABB transformed_bounds = bounds_;
		transformed_bounds.x_min -= space.x_min;
		transformed_bounds.x_max -= space.x_min;
		transformed_bounds.y_min -= space.y_min;
		transformed_bounds.y_max -= space.y_min;
		transformed_bounds *= ratio;

		for (size_t i = 0; i < clips_.size(); ++i)
		{
			for (size_t j = 0; j < clips_[0].size(); ++j)
			{
				//float mag = (10 * log10(abs(clips_[i][j].BinMagnitude(wnd_state.freq_bin))) + 10) / 20;
				float mag = abs(clips_[i][j].BinMagnitude(wnd_state.freq_bin)) / 100.0;
				//float mag = (10 * log10(abs(clips_[i][j].BinMagnitude(wnd_state.freq_bin))) + 1.0) / 1.0;

				mag = std::max<float>(0, std::min<float>(mag, 1));
				uint8_t mag_i = std::min<int>(256 * mag, 255);
				static_cast<uint32_t*>(wnd_state.bitmap_memory)[(i + static_cast<size_t>(transformed_bounds.x_min)) + (j + static_cast<size_t>(transformed_bounds.y_min)) * wnd_state.bitmap_info.bmiHeader.biWidth] = 256 * mag_i;
			}
		}

		DrawLine(wnd_state, { transformed_bounds.x_min, transformed_bounds.y_min }, { transformed_bounds.x_min, transformed_bounds.y_max });
		DrawLine(wnd_state, { transformed_bounds.x_min, transformed_bounds.y_max }, { transformed_bounds.x_max, transformed_bounds.y_max });
		DrawLine(wnd_state, { transformed_bounds.x_max, transformed_bounds.y_max }, { transformed_bounds.x_max, transformed_bounds.y_min });
		DrawLine(wnd_state, { transformed_bounds.x_max, transformed_bounds.y_min }, { transformed_bounds.x_min, transformed_bounds.y_min });
	}

	double getGridSpacing() const { return grid_spacing_; }

	Ray Hit(Ray r)
	{
		auto [x, y] = r.position / grid_spacing_;
		double dt = std::numeric_limits<double>::infinity();
		double temp = std::numeric_limits<double>::infinity();
		bool entered = false;
		Vec2 centre;
		if (abs(x - round(x)) < std::numeric_limits<float>::epsilon())
		{
			entered = true;
			temp = (0 < r.direction.x) - (r.direction.x < 0);
			centre.x = round(x) + temp / 2.0;
			temp -= x - round(x);
		}
		else
		{
			if (r.direction.x > 0)
			{
				temp = 1 - (x - floor(x));
			}
			else
			{
				temp = floor(x) - x;
			}
			centre.x = floor(x) + 0.5;
		}
		dt = temp / r.direction.x;
		if (abs(y - round(y)) < std::numeric_limits<float>::epsilon())
		{
			entered = true;
			temp = (0 < r.direction.y) - (r.direction.y < 0);
			centre.y = round(y) + temp / 2.0;
		}
		else
		{
			if (r.direction.y > 0)
			{
				temp = 1 - (y - floor(y));
			}
			else
			{
				temp = floor(y) - y;
			}
			centre.y = floor(y) + 0.5;
		}
		dt = min(dt, temp / r.direction.y);
		dt *= grid_spacing_;
		if (entered)
		{
			double t_at_centre = (centre * grid_spacing_ - r.position + r.time * SPEED_OF_SOUND * r.direction).Magnitude() / SPEED_OF_SOUND;
			size_t x_i = static_cast<size_t>(centre.x - bounds_.x_min / grid_spacing_);
			size_t y_i = static_cast<size_t>(centre.y - bounds_.y_min / grid_spacing_);
			if (x_i < 0 || x_i >= clips_.size() || y_i < 0 || y_i >= clips_[0].size())
			{
				return { {0, 0}, {0, 0}, std::numeric_limits<double>::infinity(), 0 };
			}
			clips_[x_i][y_i].Add(t_at_centre, r.amplitude);
		}
		return { r.position + r.direction * dt, r.direction, r.time + dt / SPEED_OF_SOUND, r.amplitude };
	}

private:
	double const grid_spacing_{ SPEED_OF_SOUND / SAMPLE_RATE };
	std::vector<std::vector<IR>> clips_{};

	AABB bounds_;
};
