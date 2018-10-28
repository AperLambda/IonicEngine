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

#define IONIC_MAX_GLYPHS 10023

namespace ionicengine
{
	using namespace std::rel_ops;

	struct Character
	{
		uint32_t codepoint;
		uint32_t texture_id;
		glm::ivec2 size;
		// Coords of the glyph in the texture atlas.
		glm::ivec2 position;
		// Left & top bearing when rendering.
		glm::ivec2 bearing;
		// X advance when rendering.
		uint32_t advance;

		bool operator==(const Character &rhs) const;

		bool operator<(const Character &rhs) const;
	};

	class Font
	{
	private:
		uint32_t _texture_id{};
		std::pair<uint32_t, uint32_t> _texture_size;
		std::map<char, Character> _chars;
		uint32_t _size;
		uint32_t _tab_size{4};

	public:
		Font(uint32_t textureId, std::pair<uint32_t, uint32_t> textureSize, const std::map<char, Character> &charactersMap, uint32_t size,
			 uint32_t tabSize = 4);

		Font(const Font &font);

		Font(Font &&font) noexcept;

		uint32_t get_texture_id() const;

		const std::pair<uint32_t, uint32_t> &get_texture_size() const;

		/*!
		 * Gets the character's data of the specified character.
		 * @param c The specified character.
		 * @return The data of the specified character.
		 */
		Character get_character(char c) const;

		/*!
		 * Gets the size of the font.
		 * @return The size of the font.
		 */
		uint32_t get_size() const;

		/*!
		 * Gets the tab size in space character.
		 * @return The tab size.
		 */
		uint32_t get_tab_size() const;

		/*!
		 * Sets the tab size in space character.
		 * @param tabSize The tab size.
		 */
		void set_tab_size(uint32_t tabSize);

		/*!
		 * Gets the length of a string in pixels.
		 * @param text Text to measure.
		 * @return The length of the specified string.
		 */
		uint32_t get_text_length(const std::string &text) const;

		/*!
		 * Trims the given text to a given length.
		 * @param input The given text to trim.
		 * @param length The length to trim.
		 * @param reverse Is reversed?
		 * @return The trimmed string.
		 */
		std::string trim_text_to_length(const std::string &input, uint32_t length, bool reverse = false) const;

		/*!
		 * Trims the given text to a given length and adds three dots at the end if it was trimmed.
		 * @param input The given text to trim.
		 * @param length The length to trim.
		 * @return The trimmed string.
		 */
		std::string trim_text_to_length_dotted(const std::string &input, uint32_t length) const;

		/*!
		 * Gets the height a=of a string in pixels.
		 * @param text Text to measure.
		 * @return The height of the specified string.
		 */
		uint32_t get_text_height(const std::string &text) const;

		/*!
		 * Gets general character height with margin.
		 * @return The general height.
		 */
		uint32_t get_height() const;

		bool operator==(const Font &font) const;

		bool operator<(const Font &font) const;
	};

	class FontManager
	{
	private:
		FT_Library ft;
		lambdacommon::ResourceName default_font{"liberation:fonts/sans"};

	public:
		FontManager();

		~FontManager();

		void shutdown();

		/*!
		 * Gets the default font name.
		 * @return The default font name.
		 */
		lambdacommon::ResourceName get_default_font_name() const;

		/*!
		 * Gets the default font.
		 * @return The pointer of the default font.
		 */
		Font *get_default_font() const;

		/*!
		 * Gets a loaded font.
		 * @param font_name The name of the font.
		 * @return The pointer of the font.
		 */
		Font *get_font(const lambdacommon::ResourceName &font_name) const;

		/*!
		 * Loads the font with the specified resource name.
		 * @param font_name The font's resource name.
		 * @param size The font size to load.
		 * @return An optional Font.
		 */
		std::optional<Font> load_font(const lambdacommon::ResourceName &font_name, uint32_t size) const;

		/*!
		 * Loads the font at the specified path.
		 * @param font_name The font's resource name.
		 * @param path The path where the font file resides.
		 * @param size The font size to load.
		 * @return An optional Font.
		 */
		std::optional<Font> load_font(const lambdacommon::ResourceName &font_name,
									  const lambdacommon::fs::FilePath &path,
									  uint32_t size) const;

		/*!
		 * Loads the font at the specified path.
		 * @param font_name The font's resource name.
		 * @param path The path where the font file resides.
		 * @param size The font size to load.
		 * @return An optional Font.
		 */
		std::optional<Font>
		load_font(const lambdacommon::ResourceName &font_name, const std::string &path, uint32_t size) const;
	};
}

#endif //IONICENGINE_FONT_H