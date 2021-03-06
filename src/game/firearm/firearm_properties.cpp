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


#include "../../reference/container.hpp"
#include "../../util/file.hpp"
#include "firearm.hpp"
#include <fstream>
#include <jsonpp/parser.hpp>
#include <stdexcept>


static std::map<std::string, firearm_properties> all_firearm_properties;


static void load_all(std::map<std::string, firearm_properties> & all_props);
static std::pair<std::string, firearm_properties> load_single(std::string && filename);
static firearm_properties::fire_mode_t fire_mode_from_string(const std::string & name);


const std::map<std::string, firearm_properties> & firearm::properties() {
	load_all(all_firearm_properties);
	return all_firearm_properties;
}


static void load_all(std::map<std::string, firearm_properties> & all_props) {
	for(auto && fname : list_files(firearm_root))
		all_props.insert(load_single(firearm_root + '/' + fname));
}

static std::pair<std::string, firearm_properties> load_single(std::string && filename) {
	std::ifstream gun_file(filename);
	json::value doc;
	json::parse(gun_file, doc);

	auto && sounds_v = doc["sounds"];
	std::vector<json::value> raw_sounds;
	if(!sounds_v.is<json::null>())
		raw_sounds = sounds_v["shoot"].as<std::vector<json::value>>();
	std::vector<std::string> sounds;
	sounds.reserve(raw_sounds.size());
	std::transform(raw_sounds.begin(), raw_sounds.end(), std::back_inserter(sounds), [](auto && rs) { return rs.template as<std::string>(); });

	std::string reload_sound;
	if(!sounds_v.is<json::null>())
		reload_sound = sounds_v["reload"].as<std::string>();

	unsigned int projectiles_per_shot = 1;
	if(!doc["projectiles_per_shot"].is<json::null>())
		projectiles_per_shot = std::max(doc["projectiles_per_shot"].as<unsigned int>(), 1u);

	auto have_spread = !doc["spread"].is<json::null>();
	float spread_min = 0;
	float spread_max = 0;
	if(have_spread) {
		auto spread = doc["spread"].as<json::object>();
		have_spread = spread["on"].as<bool>();
		spread_min = spread["min"].as<float>();
		spread_max = spread["max"].as<float>();
	}


	auto bullet   = doc["bullet"].as<json::object>();
	const auto id = doc["id"].as<std::string>();
	return {id,
	        {id,
	         doc["name"].as<std::string>(),
	         {
	             bullet["speed"].as<float>(),  //
	             bullet["speed_loss"].as<float>(),
	         },
	         fire_mode_from_string(doc["fire_mode"].as<std::string>()),
	         doc["action_speed"].as<float>(),
	         doc["reload_speed"].as<float>(),
	         doc["mag_size"].as<unsigned int>(),
	         doc["mag_quantity"].as<unsigned int>(),
	         {have_spread,
	          {
	              spread_min,  //
	              spread_max,
	          }},
	         projectiles_per_shot,
	         sounds,
	         reload_sound}};
}

firearm_properties::fire_mode_t fire_mode_from_string(const std::string & name) {
	if(name == "full-auto")
		return firearm_properties::fire_mode_t::full_auto;
	else if(name == "semi-auto" || name == "bolt-action" || name == "single-shot")
		return firearm_properties::fire_mode_t::semi_auto;
	else if(name == "semi-auto-response-trigger")
		return firearm_properties::fire_mode_t::semi_auto_response_trigger;
	else
		throw std::domain_error("\"" + name + "\" is not a fire_mode");
}
