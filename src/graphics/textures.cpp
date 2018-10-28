/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/textures.h"
#include "../../include/ionicengine/ionicengine.h"

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <stdexcept>

namespace ionicengine
{
	/*
	 * TextureRegion
	 */

	TextureRegion::TextureRegion(float min_x, float min_y, float max_x, float max_y) : _min_x(min_x), _min_y(min_y),
																				   _max_x(max_x), _max_y(max_y)
	{}

	float TextureRegion::min_x() const
	{
		return _min_x;
	}

	float TextureRegion::min_y() const
	{
		return _min_y;
	}

	float TextureRegion::max_x() const
	{
		return _max_x;
	}

	float TextureRegion::max_y() const
	{
		return _max_y;
	}

	void TextureRegion::min_x(float min_x)
	{
		_min_x = min_x;
	}

	void TextureRegion::min_y(float min_y)
	{
		_min_y = min_y;
	}

	void TextureRegion::max_x(float max_x)
	{
		_max_x = max_x;
	}

	void TextureRegion::max_y(float max_y)
	{
		_max_y = max_y;
	}

	bool TextureRegion::operator==(const TextureRegion &other) const
	{
		return _min_x == other._min_x && _min_y == other._min_y && _max_x == other._max_x && _max_y == other._max_y;
	}

	bool TextureRegion::operator<(const TextureRegion &other) const
	{
		return std::tie(_min_x, _min_y, _max_x, _max_y) < std::tie(other._min_x, other._min_y, other._max_x, other._max_y);
	}

	const TextureRegion TextureRegion::BASE{0.f, 0.f, 1.f, 1.f};

	/*
	 * Texture
	 */

	Texture::Texture(uint32_t id, uint32_t width, uint32_t height, uint32_t channels) : _id(id), _width(width),
																						_height(height),
																						_channels(channels)
	{}

	uint32_t Texture::get_id() const
	{
		return _id;
	}

	uint32_t Texture::get_width() const
	{
		return _width;
	}

	uint32_t Texture::get_height() const
	{
		return _height;
	}

	uint32_t Texture::get_channels() const
	{
		return _channels;
	}

	TextureRegion Texture::new_region(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		return {(static_cast<float>(x) / _width), (static_cast<float>(y) / _height),
				(static_cast<float>(x + width) / _width), (static_cast<float>(y + height) / _height)};
	}

	void Texture::bind(uint32_t texture_unit) const
	{
		glActiveTexture(texture_unit);
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void Texture::unbind() const
	{
		texture::unbind();
	}

	void Texture::delete_texture()
	{
		unbind();
		GLuint textures[] = {static_cast<GLuint>(_id)};
		glDeleteTextures(1, textures);
		_id = 0;
	}

	bool Texture::operator==(const Texture &other) const
	{
		return _id == other._id;
	}

	bool Texture::operator<(const Texture &other) const
	{
		return _id < other._id;
	}

	Texture::operator bool() const
	{
		return _id != 0;
	}

	namespace texture
	{
		std::map<lambdacommon::ResourceName, Texture> textures;

		std::optional<Texture> IONICENGINE_API
		load(const lambdacommon::ResourceName &name, const std::string &extension, TextureWrapMode wrap_mode,
			 TextureFilterMode filter_mode, bool use_mipmap)
		{
			if (!get_resources_manager().does_resource_exist(name, extension))
				return std::nullopt;

			//stbi_set_flip_vertically_on_load(true);

			int width, height, channels;
			auto image = stbi_load(get_resources_manager().get_resource_path(name, extension).to_string().c_str(), &width,
								   &height, &channels, 4);
			if (!image)
				return {};
			auto texture = create(name, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height),
								  static_cast<uint32_t>(channels), wrap_mode, filter_mode, use_mipmap);
			stbi_image_free(image);
			return texture;
		}

		Texture IONICENGINE_API
		create(const lambdacommon::ResourceName &name, unsigned char *image, uint32_t width, uint32_t height,
			   uint32_t channels, TextureWrapMode wrap_mode, TextureFilterMode filter_mode, bool use_mipmap)
		{
			uint32_t id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_mode);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			if (use_mipmap)
				glGenerateMipmap(GL_TEXTURE_2D);

			unbind();

			Texture texture{id, width, height, channels};

			textures.insert({name, texture});

			print_debug("[IonicEngine] Texture '" + name.to_string() + "' loaded successfully with ID '" +
						std::to_string(id) + "'!");

			return texture;
		}

		Texture IONICENGINE_API get_texture(const lambdacommon::ResourceName &name)
		{
			return textures.at(name);
		}

		bool IONICENGINE_API has_texture(const lambdacommon::ResourceName &name)
		{
			return textures.count(name);
		}

		TextureRegion IONICENGINE_API
		new_texture_region(uint32_t textureWidth, uint32_t textureHeight, uint32_t x, uint32_t y, uint32_t width,
						   uint32_t height)
		{
			return {(static_cast<float>(x) / textureWidth), (static_cast<float>(y) / textureHeight),
					(static_cast<float>(x + width) / textureWidth), (static_cast<float>(y + height) / textureHeight)};
		}

		void IONICENGINE_API delete_texture(const lambdacommon::ResourceName &name)
		{
			if (!has_texture(name))
				return;
			get_texture(name).delete_texture();
		}

		void IONICENGINE_API bind(uint32_t id)
		{
			glBindTexture(GL_TEXTURE_2D, id);
		}

		void IONICENGINE_API unbind()
		{
			bind(0);
		}

		void IONICENGINE_API shutdown()
		{
			for(auto const& [key, val] : textures)
			{
				delete_texture(key);
			}
		}
	}
}