// The MIT License (MIT)

// Copyright (c) 2016 nabijaczleweli

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#pragma once


#include <string>
#include <utility>
#include <vector>


struct bullet_properties {
	float speed;
	float speed_loss;
};

struct spread {
	float min;
	float max;
};

struct firearm_properties {
	enum class fire_mode_t {
		full_auto,
		semi_auto,
		// https://www.youtube.com/watch?v=PYThNUVZrYM
		semi_auto_response_trigger,
		shotgun,
	};


	std::string id;
	std::string name;
	bullet_properties bullet_props;
	fire_mode_t fire_mode;
	float action_speed;
	float reload_speed;
	unsigned int mag_size;
	unsigned int mag_quantity;
	std::pair<bool, spread> spread;
	unsigned int projectiles_per_shot;
	std::vector<std::string> shoot_sounds;
	std::string reload_sound;
};
