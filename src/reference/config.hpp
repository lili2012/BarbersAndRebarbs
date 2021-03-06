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
#include <vector>


class config {
public:
	std::string language      = "en_US";
	float controller_deadzone = 10;
	bool use_network          = true;

	bool vsync                 = true;
	unsigned int FPS           = 60;
	bool play_sounds           = true;
	unsigned int splash_length = 2;

	float player_speed                   = 1;
	float player_seconds_to_full_speed   = .4f;
	std::string player_default_firearm   = "default";
	unsigned int player_gun_popup_length = 3;

	float music_volume        = .8f;
	float sound_effect_volume = 1.f;


	static std::vector<std::string> available_languages();


	config(std::string && path);
	~config();

private:
	std::string path;
};
