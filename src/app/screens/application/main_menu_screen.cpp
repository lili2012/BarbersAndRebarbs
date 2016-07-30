// The MIT License (MIT)

// Copyright (c) 2015 nabijaczleweli

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


#include "main_menu_screen.hpp"
#include "../../../reference/container.hpp"
#include "../../../reference/joystick_info.hpp"
#include "../../../util/file.hpp"
#include "../../../util/url.hpp"
#include "../../application.hpp"
#include "../game/main_game_screen.hpp"
#include <cmath>
#include <fstream>
#include <iostream>
#include <jsonpp/parser.hpp>
#include <semver-utils/version.hpp>
#include <zstd/common/zstd.h>


using namespace std::literals;


void main_menu_screen::move_selection(main_menu_screen::direction dir) {
	switch(dir) {
		case direction::up:
			selected = std::min(main_buttons.size() - 1, selected + 1);
			break;
		case direction::down:
			if(selected)
				--selected;
			break;
	}
}

void main_menu_screen::press_button() {
	auto itr = main_buttons.begin();
	advance(itr, selected);
	(itr->second)(itr->first);
}

void main_menu_screen::try_drawings() {
	if(!(control_frames_counter++ % 10)) {
		control_frames_counter = 0;
		if(joystick_drawing.first != sf::Joystick::isConnected(0)) {
			joystick_drawing.first ^= 1;
			joystick_drawing.second.move(0, joystick_drawing.second.size().y * .55f * (joystick_drawing.first ? 1 : -1));
			keys_drawing.move(0, joystick_drawing.second.size().y * .55f * (joystick_drawing.first ? -1 : 1));
		}
	}
}

void main_menu_screen::load_game(sf::Text & txt, const std::string & save_path) {
	std::uint64_t raw_size;
	std::uint64_t compressed_size;
	std::ifstream save_file(save_path, std::ios::binary);
	if(!save_file.is_open()) {
		txt.setString("Save file inacessible");
		return;
	}

	save_file.read(reinterpret_cast<char *>(&raw_size), sizeof(raw_size)).read(reinterpret_cast<char *>(&compressed_size), sizeof(compressed_size));
	auto in_c = std::make_unique<std::uint8_t[]>(compressed_size);
	save_file.read(reinterpret_cast<char *>(in_c.get()), compressed_size);

	std::string in(raw_size, '\0');
	const auto result = ZSTD_decompress(&in[0], in.size(), in_c.get(), compressed_size);
	if(ZSTD_isError(result)) {
		txt.setString("Decompression error: "s + ZSTD_getErrorName(result));
		return;
	}

	json::value save;
	json::parse(in, save);
	app.schedule_screen<main_game_screen>(save.as<json::object>());
}


void main_menu_screen::set_default_menu_items() {
	main_buttons.clear();

	main_buttons.emplace_front(sf::Text(global_iser.translate_key("gui.application.text.start"), font_swirly),
	                           [&](sf::Text &) { app.schedule_screen<main_game_screen>(); });
	main_buttons.emplace_front(sf::Text(global_iser.translate_key("gui.application.text.load"), font_swirly), [&](sf::Text & txt) {
		main_buttons.clear();
		main_buttons.emplace_front(sf::Text(global_iser.translate_key("gui.application.text.back"), font_swirly), [&](sf::Text &) { set_default_menu_items(); });
		for(auto && fname : list_files(saves_root))
			main_buttons.emplace_front(sf::Text(fname.substr(0, fname.rfind('.')), font_swirly),
			                           [&, fname = fname ](sf::Text &) { load_game(txt, saves_root + '/' + fname); });
		selected = main_buttons.size() - 1;
	});
	main_buttons.emplace_front(sf::Text(global_iser.translate_key("gui.application.text."s + (app_configuration.play_sounds ? "" : "un") + "mute"), font_swirly),
	                           [&](sf::Text & txt) {
		                           app_configuration.play_sounds = !app_configuration.play_sounds;
		                           txt.setString(global_iser.translate_key("gui.application.text."s + (app_configuration.play_sounds ? "" : "un") + "mute"));
		                           app.retry_music();
		                         });
	main_buttons.emplace_front(sf::Text(global_iser.translate_key("gui.application.text.quit"), font_swirly), [&](sf::Text &) { app.window.close(); });

	selected = main_buttons.size() - 1;
}

void main_menu_screen::setup() {
	screen::setup();
}

int main_menu_screen::loop() {
	const auto & winsize = app.window.getSize();

	unsigned int buttid = 0;
	for(auto & button : main_buttons) {
		const auto & btnbds = button.first.getGlobalBounds();

		button.first.setPosition((winsize.x * (59.f / 60.f)) - btnbds.width,
		                         (winsize.y * (7.f / 8.f)) - (buttid + 1) * btnbds.height - (winsize.y * ((buttid * 1.f) / 90.f)));
		button.first.setColor((buttid == selected) ? sf::Color::Red : sf::Color::White);
		++buttid;
	}

	if(std::get<3>(update) && std::get<0>(update).valid()) {
		std::get<3>(update) = false;
		std::get<1>(update) = std::thread([&] {
			auto result = std::get<0>(update).get();
			std::cout << result.header["X-RateLimit-Remaining"] << " GitHub API accesses left\n";

			if(!(result.status_code >= 200 && result.status_code < 300))
				std::get<2>(update).setString("Couldn't get update data, HTTP error " + std::to_string(result.status_code));
			else {
				json::value newest_update;
				json::parse(result.text, newest_update);

				const auto new_version_s   = newest_update["tag_name"].as<std::string>();
				const auto new_version     = semver::version::from_string(new_version_s.substr(new_version_s.find_first_of("0123456789")));
				const auto current_version = semver::version::from_string(BARBERSANDREBARBS_VERSION);

				if(new_version < current_version || new_version == current_version)
					std::get<2>(update).setString("No updates found");
				else {
					std::get<2>(update).setString("Found update: " + new_version.str());

					main_buttons.emplace_back(sf::Text(global_iser.translate_key("gui.application.text.update"), font_swirly),
					                          [&, url = newest_update["html_url"].as<std::string>() ](sf::Text &) {
						                          if(!launch_browser(url)) {
							                          main_buttons.clear();
							                          main_buttons.emplace_front(sf::Text(global_iser.translate_key("gui.application.text.back"), font_swirly),
							                                                     [&](sf::Text &) { set_default_menu_items(); });
							                          main_buttons.emplace_front(sf::Text("Failed to open browser, go to", font_pixelish, 20), [&](sf::Text &) {});
							                          main_buttons.emplace_front(sf::Text(url, font_pixelish, 20),
							                                                     [&](sf::Text & txt) { copy_to_clipboard(txt.getString()); });
							                          main_buttons.emplace_front(sf::Text("to download update", font_pixelish, 20), [&](sf::Text &) {});
							                          selected = main_buttons.size() - 1;
						                          }
						                        });
				}
			}
		});
	}

	return 0;
}

int main_menu_screen::draw() {
	for(const auto & button : main_buttons)
		app.window.draw(button.first);

	try_drawings();
	if(joystick_drawing.first)
		app.window.draw(joystick_drawing.second);
	app.window.draw(keys_drawing);

	app.window.draw(std::get<2>(update));

	return 0;
}

int main_menu_screen::handle_event(const sf::Event & event) {
	if(int i = screen::handle_event(event))
		return i;

	switch(static_cast<int>(event.type)) {
		case sf::Event::MouseMoved: {
			unsigned int buttid = 0;
			for(const auto & button : main_buttons) {
				if(button.first.getGlobalBounds().contains(event.mouseMove.x, event.mouseMove.y)) {
					selected = buttid;
					break;
				}
				++buttid;
			}
		} break;

		case sf::Event::MouseButtonPressed:
			if(event.mouseButton.button == sf::Mouse::Left)
				for(const auto & button : main_buttons)
					if(button.first.getGlobalBounds().contains(event.mouseButton.x, event.mouseButton.y)) {
						press_button();
						break;
					}
			break;

		case sf::Event::MouseWheelMoved:
			move_selection((event.mouseWheel.delta > 0) ? direction::up : direction::down);
			break;
		case sf::Event::KeyPressed:
			switch(static_cast<int>(event.key.code)) {
				case sf::Keyboard::Key::Up:
					move_selection(direction::up);
					break;
				case sf::Keyboard::Key::Down:
					move_selection(direction::down);
					break;
				case sf::Keyboard::Key::Return:
				case sf::Keyboard::Key::Space:
					press_button();
					break;
			}
			break;

		case sf::Event::JoystickMoved:
			if(event.joystickMove.axis == X360_axis_mappings::LeftStickVertical || event.joystickMove.axis == X360_axis_mappings::DPadVertical) {
				if(event.joystickMove.position && (event.joystickMove.position >= 25 || event.joystickMove.position <= -25)) {
					if(joystick_up)
						break;
					joystick_up    = true;
					const int sign = event.joystickMove.position / std::abs(event.joystickMove.position);

					move_selection((sign == ((event.joystickMove.axis == X360_axis_mappings::LeftStickVertical) ? X360_axis_up_right_mappings::LeftStickVertical
					                                                                                            : X360_axis_up_right_mappings::DPadVertical))
					                   ? direction::up
					                   : direction::down);
				} else
					joystick_up = false;
			}
			break;

		case sf::Event::JoystickButtonPressed:
			if(event.joystickButton.button == X360_button_mappings::A)
				press_button();
			break;
	}

	return 0;
}

main_menu_screen::main_menu_screen(application & theapp)
      : screen(theapp), control_frames_counter(0), joystick_up(false), joystick_drawing(false, drawing("xbox", app.window.getSize())),
        keys_drawing("keyboard", app.window.getSize()),
        update(cpr::GetAsync(cpr::Url("https://api.github.com/repos/nabijaczleweli/BarbersAndRebarbs/releases/latest"), cpr::Parameters{{"anon", "true"}}),
               std::thread(), sf::Text("", font_monospace, 10), true) {
	keys_drawing.move(app.window.getSize().x / 4 - keys_drawing.size().x / 2, app.window.getSize().y / 2 - keys_drawing.size().y / 2);
	joystick_drawing.second.move(app.window.getSize().x / 4 - joystick_drawing.second.size().x / 2,
	                             app.window.getSize().y / 2 - joystick_drawing.second.size().y / 2);

	set_default_menu_items();
}

main_menu_screen::~main_menu_screen() {
	if(std::get<1>(update).joinable())
		std::get<1>(update).join();
}
