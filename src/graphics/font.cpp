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
#include <lambdacommon/maths.h>
#include <sstream>

namespace maths = lambdacommon::maths;

namespace ionicengine
{
	bool Character::operator==(const Character &rhs) const
	{
		return codepoint == rhs.codepoint &&
			   texture_id == rhs.texture_id &&
			   advance == rhs.advance;
	}

	bool Character::operator<(const Character &rhs) const
	{
		return std::tie(codepoint, texture_id, advance) < std::tie(rhs.codepoint, rhs.texture_id, rhs.advance);
	}

	Font::Font(uint32_t textureId, std::pair<uint32_t, uint32_t> textureSize,
			   const std::map<char, Character> &charactersMap, uint32_t size, uint32_t tabSize)
			: _texture_id(textureId),
			  _texture_size(textureSize),
			  _chars(charactersMap),
			  _size(size),
			  _tab_size(tabSize)
	{}

	Font::Font(const Font &font) = default;

	Font::Font(Font &&font) noexcept : _texture_id(font._texture_id), _chars(std::move(font._chars)), _size(font._size),
									   _tab_size(font._tab_size)
	{}

	uint32_t Font::get_texture_id() const
	{
		return _texture_id;
	}

	const std::pair<uint32_t, uint32_t> &Font::get_texture_size() const
	{
		return _texture_size;
	}

	Character Font::get_character(char c) const
	{
		if (!_chars.count(c))
			return Character{};
		return _chars.at(c);
	}

	uint32_t Font::get_size() const
	{
		return _size;
	}

	uint32_t Font::get_tab_size() const
	{
		return _tab_size;
	}

	void Font::set_tab_size(uint32_t tabSize)
	{
		_tab_size = tabSize;
	}

	uint32_t Font::get_text_length(const std::string &text) const
	{
		if (text.empty())
			return 0;
		uint32_t length{0};
		std::string tab;
		for (uint32_t i = 0; i < get_tab_size(); i++)
			tab += " ";
		auto reformatted_text = lambdacommon::lstring::replace_all(text, "\t", tab);
		for (const auto &line : lambdacommon::lstring::split(reformatted_text, '\n'))
		{
			uint32_t x{0};
			std::string::const_iterator c;
			for (c = line.begin(); c != line.end(); c++)
			{
				x += (get_character(*c).advance >> 6);
			}
			if (length < x)
				length = x;
		}
		return length;
	}

	std::string Font::trim_text_to_length(const std::string &input, uint32_t length, bool reverse) const
	{
		std::stringstream ss;
		uint32_t i = 0;
		int j = reverse ? static_cast<int>(input.length()) - 1 : 0;
		int increment = reverse ? -1 : 1;

		for (int l = j; l >= 0 && l < input.length() && i < length; l += increment)
		{
			char current_char = input.at(l);
			uint32_t width = get_character(current_char).advance >> 6;

			i += width;

			if (reverse)
			{
				const std::string &temp = ss.str();
				ss.seekp(0);
				ss << current_char;
				ss << temp;
			}
			else
				ss << current_char;
		}

		return ss.str();
	}

	std::string Font::trim_text_to_length_dotted(const std::string &input, uint32_t length) const
	{
		auto trimmed_text = trim_text_to_length(input, length);
		if (trimmed_text != input)
			trimmed_text = trimmed_text.substr(0, trimmed_text.length() - 3) + "...";

		return trimmed_text;
	}

	uint32_t Font::get_text_height(const std::string &text) const
	{
		return static_cast<uint32_t>(lambdacommon::lstring::split(text, '\n').size() * get_height());
	}

	uint32_t Font::get_height() const
	{
		auto h_char = get_character('H');
		auto g_char = get_character('g');
		auto below_origin = static_cast<uint32_t>(g_char.size.y - g_char.bearing.y);
		auto max_bearing_y = h_char.bearing.y;
		return below_origin + max_bearing_y + 4;
	}

	bool Font::operator==(const Font &font) const
	{
		return _chars == font._chars &&
			   _size == font._size &&
			   _tab_size == font._tab_size;
	}

	bool Font::operator<(const Font &font) const
	{
		return std::tie(_chars, _size, _tab_size) < std::tie(font._chars, font._size, font._tab_size);
	}

	std::map<lambdacommon::ResourceName, Font *> fonts;

	FontManager::FontManager()
	{
		if (FT_Init_FreeType(&ft))
			throw std::runtime_error("Cannot initialize FreeType library!");

#ifdef LAMBDA_WINDOWS
		default_font = {"windows:fonts/arial"};
		if (!this->load_font(default_font, std::string("C:\\Windows\\Fonts\\arial.ttf"), 12))
			throw std::runtime_error("Cannot load arial.ttf");
#else
		if (!this->loadFont(default_font, std::string("LiberationSans-Regular.ttf"), 12))
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

	lambdacommon::ResourceName FontManager::get_default_font_name() const
	{
		return default_font;
	}

	Font *FontManager::get_default_font() const
	{
		return get_font(default_font);
	}

	Font *FontManager::get_font(const lambdacommon::ResourceName &font_name) const
	{
		return fonts.at(font_name);
	}

	std::optional<Font> FontManager::load_font(const lambdacommon::ResourceName &font_name, uint32_t size) const
	{
		if (!get_resources_manager().does_resource_exist(font_name, "ttf"))
			return std::nullopt;
		return load_font(font_name, get_resources_manager().get_resource_path(font_name, "ttf"), size);
	}

	std::optional<Font>
	FontManager::load_font(const lambdacommon::ResourceName &font_name, const lambdacommon::fs::FilePath &path,
						   uint32_t size) const
	{
		return load_font(font_name, path.to_string(), size);
	}

#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>

	std::optional<Font>
	FontManager::load_font(const lambdacommon::ResourceName &font_name, const std::string &path, uint32_t size) const
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

		std::map<char, Character> characters_map;

		// Disable byte-alignment restriction
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		FT_GlyphSlot g = face->glyph;
		uint32_t w = 0;
		uint32_t h = 0;

		for (uint32_t c = 0; c < IONIC_MAX_GLYPHS; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
				continue;

			w += face->glyph->bitmap.width;
			h = maths::max(h, face->glyph->bitmap.rows);
		}

		uint32_t texture_atlas;
		glGenTextures(1, &texture_atlas);
		texture::bind(texture_atlas);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_ALPHA, GL_UNSIGNED_BYTE, nullptr);
		// Set texture options.
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		uint32_t x = 0;

		for (uint32_t c = 0; c < IONIC_MAX_GLYPHS; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				print_error("Failed to load Glyph '" + std::to_string(c) + "'.");
				continue;
			}

			texture::bind(texture_atlas);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, g->bitmap.width, g->bitmap.rows, GL_ALPHA, GL_UNSIGNED_BYTE,
							g->bitmap.buffer);

			// Generate texture.
			uint32_t texture;
			glGenTextures(1, &texture);
			texture::bind(texture);
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
					{g->bitmap.width, g->bitmap.rows},
					{g->bitmap_left, g->bitmap_top},
					glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
					static_cast<uint32_t>(face->glyph->advance.x)
			};
			characters_map.insert(std::pair<char, Character>(c, character));

			x += g->bitmap.width;
		}

		auto *pixels = (unsigned char *) malloc(w * h);
		glGetTexImage(GL_TEXTURE_2D, 0, GL_ALPHA, GL_UNSIGNED_BYTE, pixels);

		unsigned char *png_data = (unsigned char *) calloc(w * h * 4, 1);
		for (int i = 0; i < (w * h); ++i)
		{
			png_data[i * 4 + 0] |= 0;
			png_data[i * 4 + 1] |= 0;
			png_data[i * 4 + 2] |= 0;
			png_data[i * 4 + 3] = pixels[i];
		}

		stbi_write_png("font_output.png", w, h, 4, png_data, w * 4);

		texture::unbind();
		//hb_font_destroy(hb_ft_font);
		FT_Done_Face(face);

		Font *font = new Font{texture_atlas, {w, h}, characters_map, size};
		print_debug("[IonicEngine] Font '" + font_name.to_string() + "' at '" + path + "' loaded successfully!");
		fonts[font_name] = font;
		return {*font};
	}
}