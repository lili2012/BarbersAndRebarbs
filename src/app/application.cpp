// The MIT License (MIT)

// Copyright (c) 2014 nabijaczleweli

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

#include "application.hpp"
#include "../reference/container.hpp"
#include "../util/file.hpp"
#include "screens/application/splash_screen.hpp"
#include <algorithm>
#include <thread>
#include <SFML/System.hpp>


using namespace sf;
using namespace std;


static sequential_music * open_sequential_application_music(bool sound) {
	return (sound ? [&]() {                                                             //  If sounds shall be played
		auto files = list_files(sound_root + "/main_menu");                                 //  list all files
		files.erase(remove_if(files.begin(), files.end(), [&](const string & val) {
			return val.find("music");                                                         //  filter out all that are not prefixed with "music"
		}), files.end());
		files.shrink_to_fit();
		return files.size() ? new sequential_music(files.size(), [&](unsigned int id) {     //  if any matches
			static const auto files_music = files;                                              //  get only ones prefixed with "music"
			return sound_root + "/main_menu/" + files_music[id];
		}) : silent_music.get();                                                            //  else silent
	}() : silent_music.get());                                                          //  else silent
}


unsigned int application::effective_FPS() {
	return app_configuration.vsync ? 60 : app_configuration.FPS;
}

int application::run() {
	window.create(VideoMode::getDesktopMode(), app_name, Style::Fullscreen);
	if(app_configuration.vsync)
		window.setVerticalSyncEnabled(true);
	else
		window.setFramerateLimit(app_configuration.FPS);
	window.setMouseCursorVisible(false);
	mouse_pointer.loadFromFile(textures_root + "/gui/general/cursor.png");


	{
		Image icon;
		if(icon.loadFromFile(textures_root + "/gui/general/window_main.png"))
			window.setIcon(icon.getSize().x, icon.getSize().x, icon.getPixelsPtr());
	}

	schedule_screen<splash_screen>(splash_length * effective_FPS());
	return loop();
}

int application::loop() {
	retry_music();

	while(window.isOpen()) {
		while(temp_screen) {
			current_screen = move(temp_screen);
			current_screen->setup();
		}

		if(const int i = current_screen->loop())
			return i;
		music->tick();

		if(const int i = draw())
			return i;

		Event event;
		while(window.pollEvent(event))
			if(const int i = current_screen->handle_event(event))
				return i;
	}
	return 0;
}

int application::draw() {
	window.clear(Color::Black);

	if(const int i = current_screen->draw())
		return i;

	mouse_pointer.setPosition(static_cast<Vector2f>(Mouse::getPosition(window)));
	window.draw(mouse_pointer);

	window.display();
	return 0;
}

void application::retry_music() {
	music = open_sequential_application_music(app_configuration.play_sounds);
	music->play();
}