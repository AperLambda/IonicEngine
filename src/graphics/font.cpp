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

namespace ionicengine
{
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

	FontManager::FontManager()
	{
		if (FT_Init_FreeType(&ft))
			throw std::runtime_error("Cannot initialize FreeType library!");
	}

	FontManager::~FontManager()
	{
		FT_Done_FreeType(ft);
	}

	std::optional<Font> FontManager::loadFont(const lambdacommon::ResourceName &fontName, uint32_t size) const
	{
		if (!getResourcesManager().doesResourceExist(fontName, "ttf"))
			return std::nullopt;
		return loadFont(getResourcesManager().getResourcePath(fontName, "ttf"), size);
	}

	std::optional<Font> FontManager::loadFont(const lambdacommon::fs::FilePath &path, uint32_t size) const
	{
		return loadFont(path.toString(), size);
	}

	std::optional<Font> FontManager::loadFont(const std::string &path, uint32_t size) const
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
		for (uint32_t c = 0; c < 256; c++)
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

		return {Font{charactersMap, size}};
	}
}