/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/font.h"
#include "../../include/ionicengine/ionicengine.h"
#include "../../include/ionicengine/graphics/textures.h"
//#include <harfbuzz/hb.h>
//#include <harfbuzz/hb-ft.h>
#include <sstream>

namespace ionicengine
{
	bool Character::operator==(const Character &rhs) const
	{
		return codepoint == rhs.codepoint &&
			   textureId == rhs.textureId &&
			   advance == rhs.advance;
	}

	bool Character::operator<(const Character &rhs) const
	{
		return std::tie(codepoint, textureId, advance) < std::tie(rhs.codepoint, rhs.textureId, rhs.advance);
	}

	Font::Font(const std::map<char, Character> &charactersMap, uint32_t size, uint32_t tabSize) : _chars(charactersMap),
																								  _size(size),
																								  _tabSize(tabSize)
	{}

	Font::Font(const Font &font) = default;

	Font::Font(Font &&font) noexcept : _chars(std::move(font._chars)), _size(font._size), _tabSize(font._tabSize)
	{}

	Character Font::getCharacter(char c) const
	{
		if (!_chars.count(c))
			return Character{};
		return _chars.at(c);
	}

	uint32_t Font::getSize() const
	{
		return _size;
	}

	uint32_t Font::getTabSize() const
	{
		return _tabSize;
	}

	void Font::setTabSize(uint32_t tabSize)
	{
		_tabSize = tabSize;
	}

	uint32_t Font::getTextLength(const std::string &text) const
	{
		if (text.empty())
			return 0;
		uint32_t length{0};
		std::string tab;
		for (uint32_t i = 0; i < getTabSize(); i++)
			tab += " ";
		auto reformattedText = lambdacommon::lambdastring::replaceAll(text, "\t", tab);
		for (const auto &line : lambdacommon::lambdastring::split(reformattedText, '\n'))
		{
			uint32_t x{0};
			std::string::const_iterator c;
			for (c = line.begin(); c != line.end(); c++)
			{
				x += (getCharacter(*c).advance >> 6);
			}
			if (length < x)
				length = x;
		}
		return length;
	}

	std::string Font::trimTextToLength(const std::string &input, uint32_t length, bool reverse) const
	{
		std::stringstream ss;
		uint32_t i = 0;
		int j = reverse ? static_cast<int>(input.length()) - 1 : 0;
		int increment = reverse ? -1 : 1;

		for (int l = j; l >= 0 && l < input.length() && i < length; l += increment)
		{
			char currentChar = input.at(l);
			uint32_t width = getCharacter(currentChar).advance >> 6;

			i += width;

			if (reverse)
			{
				const std::string &temp = ss.str();
				ss.seekp(0);
				ss << currentChar;
				ss << temp;
			}
			else
				ss << currentChar;
		}

		return ss.str();
	}

	std::string Font::trimTextToLengthDotted(const std::string &input, uint32_t length) const
	{
		auto trimmedText = trimTextToLength(input, length);
		if (trimmedText != input)
			trimmedText = trimmedText.substr(0, trimmedText.length() - 3) + "...";

		return trimmedText;
	}

	uint32_t Font::getTextHeight(const std::string &text) const
	{
		return static_cast<uint32_t>(lambdacommon::lambdastring::split(text, '\n').size() * getHeight());
	}

	uint32_t Font::getHeight() const
	{
		auto hChar = getCharacter('H');
		auto gChar = getCharacter('g');
		auto belowOrigin = static_cast<uint32_t>(gChar.size.y - gChar.bearing.y);
		auto maxBearingY = hChar.bearing.y;
		return belowOrigin + maxBearingY + 4;
	}

	bool Font::operator==(const Font &font) const
	{
		return _chars == font._chars &&
			   _size == font._size &&
			   _tabSize == font._tabSize;
	}

	bool Font::operator<(const Font &font) const
	{
		return std::tie(_chars, _size, _tabSize) < std::tie(font._chars, font._size, font._tabSize);
	}

	std::map<lambdacommon::ResourceName, Font *> fonts;

	FontManager::FontManager()
	{
		if (FT_Init_FreeType(&ft))
			throw std::runtime_error("Cannot initialize FreeType library!");

#ifdef LAMBDA_WINDOWS
		defaultFont = {"windows:fonts/arial"};
		if (!this->loadFont(defaultFont, std::string("C:\\Windows\\Fonts\\arial.ttf"), 12))
			throw std::runtime_error("Cannot load arial.ttf");
#else
		if (!this->loadFont(defaultFont, std::string("LiberationSans-Regular.ttf"), 12))
			throw std::runtime_error("Cannot load LiberationSans-Regular.ttf");
#endif
	}

	FontManager::~FontManager()
	{
		FT_Done_FreeType(ft);
	}

	void FontManager::shutdown()
	{
		for (auto font : fonts)
			delete font.second;
	}

	lambdacommon::ResourceName FontManager::getDefaultFontName() const
	{
		return defaultFont;
	}

	Font *FontManager::getDefaultFont() const
	{
		return getFont(defaultFont);
	}

	Font *FontManager::getFont(const lambdacommon::ResourceName &fontName) const
	{
		return fonts.at(fontName);
	}

	std::optional<Font> FontManager::loadFont(const lambdacommon::ResourceName &fontName, uint32_t size) const
	{
		if (!getResourcesManager().doesResourceExist(fontName, "ttf"))
			return std::nullopt;
		return loadFont(fontName, getResourcesManager().getResourcePath(fontName, "ttf"), size);
	}

	std::optional<Font>
	FontManager::loadFont(const lambdacommon::ResourceName &fontName, const lambdacommon::fs::FilePath &path,
						  uint32_t size) const
	{
		return loadFont(fontName, path.toString(), size);
	}

	std::optional<Font>
	FontManager::loadFont(const lambdacommon::ResourceName &fontName, const std::string &path, uint32_t size) const
	{
		FT_Face face;
		if (FT_New_Face(ft, path.c_str(), 0, &face))
			return std::nullopt;

		if (FT_Select_Charmap(face, FT_ENCODING_UNICODE))
			return std::nullopt;

		//auto hb_ft_font = hb_ft_font_create(face, nullptr);

		// Sets the size of the font to extract.
		// Width is dynamically calculated based on the given height.
		FT_Set_Pixel_Sizes(face, 0, size);

		std::map<char, Character> charactersMap;

		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		// Load the first 256 ASCII characters. @TODO Add more characters.
		for (uint32_t c = 0; c < 10023; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				printError("Failed to load Glyph '" + std::to_string(c) + "'.");
				continue;
			}

			// Generate texture.
			uint32_t texture;
			glGenTextures(1, &texture);
			glBindTexture(GL_TEXTURE_2D, texture);
			glTexImage2D(
					GL_TEXTURE_2D,
					0,
					GL_RED,
					face->glyph->bitmap.width,
					face->glyph->bitmap.rows,
					0,
					GL_RED,
					GL_UNSIGNED_BYTE,
					face->glyph->bitmap.buffer
			);
			// Set texture options.
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// Now store character for later use.
			Character character{
					c,
					texture,
					glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<uint32_t>(face->glyph->advance.x)
			};
			charactersMap.insert(std::pair<char, Character>(c, character));
		}

		texture::unbind();
		//hb_font_destroy(hb_ft_font);
		FT_Done_Face(face);

		Font *font = new Font{charactersMap, size};
		printDebug("[IonicEngine] Font '" + fontName.toString() + "' at '" + path + "' loaded successfully!");
		fonts[fontName] = font;
		return {*font};
	}
}