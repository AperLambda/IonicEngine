/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_FONT_H
#define IONICENGINE_FONT_H

#include "ionicengine_exports.h"
#include <lambdacommon/resources.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <map>

namespace ionicengine
{
	struct Character
	{
		uint32_t textureId;
		glm::ivec2 size;
		glm::ivec2 bearing;
		uint32_t advance;
	};

	class Font
	{
	private:
		std::map<char, Character> _chars;
		uint32_t _size;

	public:
		Font(const std::map<char, Character> &charactersMap, uint32_t size);

		Font(const Font &font);

		Font(Font &&font) noexcept;

		Character getCharacter(char c) const;
	};

	class FontManager
	{
	private:
		FT_Library ft;

	public:
		FontManager();

		~FontManager();

		std::optional<Font> loadFont(const lambdacommon::ResourceName &fontName, uint32_t size) const;

		std::optional<Font> loadFont(const lambdacommon::fs::FilePath &path, uint32_t size) const;

		std::optional<Font> loadFont(const std::string &path, uint32_t size) const;
	};
}

#endif //IONICENGINE_FONT_H
