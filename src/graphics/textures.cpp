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

	TextureRegion::TextureRegion(float minX, float minY, float maxX, float maxY) : _minX(minX), _minY(minY),
																				   _maxX(maxX), _maxY(maxY)
	{}

	float TextureRegion::minX() const
	{
		return _minX;
	}

	float TextureRegion::minY() const
	{
		return _minY;
	}

	float TextureRegion::maxX() const
	{
		return _maxX;
	}

	float TextureRegion::maxY() const
	{
		return _maxY;
	}

	void TextureRegion::minX(float minX)
	{
		_minX = minX;
	}

	void TextureRegion::minY(float minY)
	{
		_minY = minY;
	}

	void TextureRegion::maxX(float maxX)
	{
		_maxX = maxX;
	}

	void TextureRegion::maxY(float maxY)
	{
		_maxY = maxY;
	}

	bool TextureRegion::operator==(const TextureRegion &other) const
	{
		return _minX == other._minX && _minY == other._minY && _maxX == other._maxX && _maxY == other._maxY;
	}

	bool TextureRegion::operator<(const TextureRegion &other) const
	{
		return std::tie(_minX, _minY, _maxX, _maxY) < std::tie(other._minX, other._minY, other._maxX, other._maxY);
	}

	const TextureRegion TextureRegion::BASE{0.f, 0.f, 1.f, 1.f};

	/*
	 * Texture
	 */

	Texture::Texture(uint32_t id, uint32_t width, uint32_t height, uint32_t channels) : _id(id), _width(width),
																						_height(height),
																						_channels(channels)
	{}

	uint32_t Texture::getId() const
	{
		return _id;
	}

	uint32_t Texture::getWidth() const
	{
		return _width;
	}

	uint32_t Texture::getHeight() const
	{
		return _height;
	}

	uint32_t Texture::getChannels() const
	{
		return _channels;
	}

	TextureRegion Texture::getRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const
	{
		return {(static_cast<float>(x) / _width), (static_cast<float>(y) / _height),
				(static_cast<float>(x + width) / _width), (static_cast<float>(y + height) / _height)};
	}

	void Texture::bind(uint32_t textureUnit) const
	{
		glActiveTexture(textureUnit);
		glBindTexture(GL_TEXTURE_2D, _id);
	}

	void Texture::unbind() const
	{
		texture::unbind();
	}

	void Texture::deleteTexture()
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
		load(const lambdacommon::ResourceName &name, const std::string &extension, TextureWrapMode wrapMode,
			 TextureFilterMode filterMode,
			 bool useMipmap)
		{
			if (!getResourcesManager().doesResourceExist(name, extension))
				return std::nullopt;

			//stbi_set_flip_vertically_on_load(true);

			int width, height, channels;
			auto image = stbi_load(getResourcesManager().getResourcePath(name, extension).toString().c_str(), &width,
								   &height, &channels, 4);
			if (!image)
				return {};
			auto texture = create(name, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height),
								  static_cast<uint32_t>(channels), wrapMode, filterMode, useMipmap);
			stbi_image_free(image);
			return texture;
		}

		Texture IONICENGINE_API
		create(const lambdacommon::ResourceName &name, unsigned char *image, uint32_t width, uint32_t height,
			   uint32_t channels, TextureWrapMode wrapMode, TextureFilterMode filterMode, bool useMipmap)
		{
			uint32_t id;
			glGenTextures(1, &id);
			glBindTexture(GL_TEXTURE_2D, id);
			// Set texture options
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
			if (useMipmap)
				glGenerateMipmap(GL_TEXTURE_2D);

			unbind();

			Texture texture{id, width, height, channels};

			textures.insert({name, texture});

			printDebug("[IonicEngine] Texture '" + name.toString() + "' loaded successfully with ID '" +
					   std::to_string(id) + "'!");

			return texture;
		}

		Texture IONICENGINE_API getTexture(const lambdacommon::ResourceName &name)
		{
			return textures.at(name);
		}

		bool IONICENGINE_API hasTexture(const lambdacommon::ResourceName &name)
		{
			return textures.count(name);
		}

		TextureRegion IONICENGINE_API
		newTextureRegion(uint32_t textureWidth, uint32_t textureHeight, uint32_t x, uint32_t y, uint32_t width,
						 uint32_t height)
		{
			return {(static_cast<float>(x) / textureWidth), (static_cast<float>(y) / textureHeight),
					(static_cast<float>(x + width) / textureWidth), (static_cast<float>(y + height) / textureHeight)};
		}

		void IONICENGINE_API deleteTexture(const lambdacommon::ResourceName &name)
		{
			if (!hasTexture(name))
				return;
			getTexture(name).deleteTexture();
		}

		void IONICENGINE_API unbind()
		{
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		void IONICENGINE_API shutdown()
		{
			for(auto const& [key, val] : textures)
			{
				deleteTexture(key);
			}
		}
	}
}