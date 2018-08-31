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

#include "../includes.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/glm.hpp>
#include <optional>
#include <map>

namespace ionicengine
{
	struct Character
	{
		uint32_t codepoint;
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
		uint32_t _tabSize{4};

	public:
		Font(const std::map<char, Character> &charactersMap, uint32_t size, uint32_t tabSize = 4);

		Font(const Font &font);

		Font(Font &&font) noexcept;

		/*!
		 * Gets the character's data of the specified character.
		 * @param c The specified character.
		 * @return The data of the specified character.
		 */
		Character getCharacter(char c) const;

		/*!
		 * Gets the size of the font.
		 * @return The size of the font.
		 */
		uint32_t getSize() const;

		/*!
		 * Gets the tab size in space character.
		 * @return The tab size.
		 */
		uint32_t getTabSize() const;

		/*!
		 * Sets the tab size in space character.
		 * @param tabSize The tab size.
		 */
		void setTabSize(uint32_t tabSize);

		/*!
		 * Gets the length of a string in pixels.
		 * @param text Text to measure.
		 * @return The length of the specified string.
		 */
		uint32_t getTextLength(const std::string &text) const;

		/*!
		 * Trims the given text to a given length.
		 * @param input The given text to trim.
		 * @param length The length to trim.
		 * @param reverse Is reversed?
		 * @return The trimmed string.
		 */
		std::string trimTextToLength(const std::string &input, uint32_t length, bool reverse = false) const;

		/*!
		 * Trims the given text to a given length and adds three dots at the end if it was trimmed.
		 * @param input The given text to trim.
		 * @param length The length to trim.
		 * @return The trimmed string.
		 */
		std::string trimTextToLengthDotted(const std::string &input, uint32_t length) const;

		/*!
		 * Gets the height a=of a string in pixels.
		 * @param text Text to measure.
		 * @return The height of the specified string.
		 */
		uint32_t getTextHeight(const std::string &text) const;

		/*!
		 * Gets general character height with margin.
		 * @return The general height.
		 */
		uint32_t getHeight() const;
	};

	class FontManager
	{
	private:
		FT_Library ft;

	public:
		FontManager();

		~FontManager();

		/*!
		 * Loads the font with the specified resource name.
		 * @param fontName The font's resource name.
		 * @param size The font size to load.
		 * @return An optional Font.
		 */
		std::optional<Font> loadFont(const lambdacommon::ResourceName &fontName, uint32_t size) const;

		/*!
		 * Loads the font at the specified path.
		 * @param path The path where the font file resides.
		 * @param size The font size to load.
		 * @return An optional Font.
		 */
		std::optional<Font> loadFont(const lambdacommon::fs::FilePath &path, uint32_t size) const;

		/*!
		 * Loads the font at the specified path.
		 * @param path The path where the font file resides.
		 * @param size The font size to load.
		 * @return An optional Font.
		 */
		std::optional<Font> loadFont(const std::string &path, uint32_t size) const;
	};
}

#endif //IONICENGINE_FONT_H
