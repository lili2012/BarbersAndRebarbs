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
#ifndef MANAGED_SPRITE_HPP
#define MANAGED_SPRITE_HPP


#include <SFML/Graphics.hpp>


class managed_sprite : public sf::Texture, public sf::Sprite {
public:
	inline virtual bool loadFromFile(const std::string & filename, const sf::IntRect & area = sf::IntRect()) {
		const auto ret = Texture::loadFromFile(filename, area);
		setTexture(*this);
		return ret;
	}

	inline virtual bool loadFromMemory(const void * data, std::size_t size, const sf::IntRect & area = sf::IntRect()) {
		const auto ret = Texture::loadFromMemory(data, size, area);
		setTexture(*this);
		return ret;
	}

	inline virtual bool loadFromStream(sf::InputStream & stream, const sf::IntRect & area = sf::IntRect()) {
		const auto ret = Texture::loadFromStream(stream, area);
		setTexture(*this);
		return ret;
	}

	inline virtual bool loadFromImage(const sf::Image & image, const sf::IntRect & area = sf::IntRect()) {
		const auto ret = Texture::loadFromImage(image, area);
		setTexture(*this);
		return ret;
	}
};


#endif  // MANAGED_SPRITE_HPP
