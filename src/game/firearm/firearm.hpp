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


#include "../world.hpp"
#include "firearm_properties.hpp"
#include <SFML/System.hpp>
#include <audiere.h>
#include <chrono>
#include <jsonpp/value.hpp>
#include <map>
#include <string>


class firearm {
private:
	const firearm_properties * props;
	game_world * world;
	/*const*/ std::chrono::high_resolution_clock::duration action_speed;
	/*const*/ std::chrono::high_resolution_clock::duration reload_speed;

	std::chrono::high_resolution_clock::time_point action_repeat_start;
	std::chrono::high_resolution_clock::time_point mag_reload_start;
	bool trigger_pulled;
	unsigned int left_in_mag;
	unsigned int left_mags;

	std::vector<audiere::SoundEffectPtr> shoot_sounds;
	std::size_t last_shoot_sound;
	audiere::SoundEffectPtr reload_sound;


	void fire(std::chrono::time_point<std::chrono::high_resolution_clock> now, float pos_x, float pos_y, const sf::Vector2f & aim);

public:
	static const std::map<std::string, firearm_properties> & properties();


	firearm();
	firearm(game_world & world, const std::string & gun_id);
	firearm(game_world & world, const json::object & from);

	void read_from_json(const json::object & from);
	json::object write_to_json() const;

	void trigger(float pos_x, float pos_y, const sf::Vector2f & aim);
	void tick(float pos_x, float pos_y, const sf::Vector2f & aim);
	void untrigger(float pos_x, float pos_y, const sf::Vector2f & aim);
	void reload();

	const std::string & id() const noexcept;
	const std::string & name() const noexcept;

	float progress() const noexcept;
	float depletion() const noexcept;
};
