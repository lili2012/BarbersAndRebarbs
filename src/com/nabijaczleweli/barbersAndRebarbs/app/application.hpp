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


#pragma once
#ifndef APPLICATION_HPP
#define APPLICATION_HPP


#include "../render/managed_sprite.hpp"
#include "../sound/sequential_music.hpp"
#include "../util/configurable.hpp"
#include "screens/screen.hpp"
#include <SFML/Graphics.hpp>


class application : configurable {
	private:
		friend screen;
		friend class main_game_screen;
		friend class main_menu_screen;
		friend class splash_screen;

		screen * current_screen = nullptr,
		       * temp_screen = nullptr;
		sf::RenderWindow window;
		managed_sprite mouse_pointer;
		SequentialMusicPtr music;
		unsigned int splash_length;

		int loop();
		int draw();

		virtual void config(cpponfig::configuration & cfg) override;

	public:
		static const unsigned int & FPS;  // 0: vsync
		static const unsigned int vsync_FPS;

		static inline unsigned int effective_FPS() __attribute__((always_inline)) {
			return FPS ?: vsync_FPS;
		}


		cpponfig::property * play_sounds = nullptr;

		int run();

		void retry_music();

		template<class T, class... A>
		inline void schedule_screen(const A &... args) {
			temp_screen = new T(this, args...);
		}

		application();
		virtual ~application();
};


#endif  // APPLICATION_HPP
