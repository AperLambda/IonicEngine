/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_TEXTURES_H
#define IONICENGINE_TEXTURES_H

#include "../includes.h"
#include <optional>

namespace ionicengine
{
	using namespace std::rel_ops;

	enum TextureWrapMode
	{
		CLAMP = 0x2900,
		CLAMP_TO_EDGE = 0x812F,
		TEXTURE_REPEAT = 0x2901
	};

	enum TextureFilterMode
	{
		NEAREST = 0x2600,
		LINEAR = 0x2601,
		NEAREST_MIPMAP_NEAREST = 0x2700,
		LINEAR_MIMAP_NEAREST = 0x2701,
		NEAREST_MIPMAP_LINEAR = 0x2702,
		LINEAR_MIPMAP_LINEAR = 0x2703
	};

	class TextureRegion
	{
	private:
		/*!
		 * Stores the top-left texture X coordinate, with a value between 0.0 and 1.0.
		 */
		float _minX;

		/*!
		 * Stores the top-left texture Y coordinate, with a value between 0.0 and 1.0.
		 */
		float _minY;

		/*!
		 * Stores the bottom-right texture X coordinate, with a value between 0.0 and 1.0.
		 */
		float _maxX;

		/*!
		 * Stores the bottom-right texture Y coordinate, with a value between 0.0 and 1.0.
		 */
		float _maxY;

	public:
		TextureRegion(float minX, float minY, float maxX, float maxY);

		/*!
		 * Gets the top-left corner texture X coordinate.
		 *
		 * @return X coordinate between 0.0 and 1.0.
		 */
		float minX() const;

		/*!
		 * Gets the top-left corner texture Y coordinate.
		 *
		 * @return Y coordinate between 0.0 and 1.0.
		 */
		float minY() const;

		/*!
		 * Gets the bottom-right corner texture X coordinate.
		 *
		 * @return X coordinate between 0.0 and 1.0.
		 */
		float maxX() const;

		/*!
		 * Gets the bottom-right corner texture Y coordinate.
		 *
		 * @return Y coordinate between 0.0 and 1.0.
		 */
		float maxY() const;

		/*!
		 * Sets the top-left corner texture X coordinate.
		 *
		 * @param minX X coordinate between 0.0 and 1.0.
		 */
		void minX(float minX);

		/*!
		 * Sets the top-left corner texture Y coordinate.
		 *
		 * @param minY Y coordinate between 0.0 and 1.0.
		 */
		void minY(float minY);

		/*!
		 * Sets the bottom-right corner texture X coordinate.
		 *
		 * @param maxX X coordinate between 0.0 and 1.0.
		 */
		void maxX(float maxX);

		/*!
		 * Sets the bottom-right corner texture Y coordinate.
		 *
		 * @param maxY Y coordinate between 0.0 and 1.0.
		 */
		void maxY(float maxY);

		bool operator==(const TextureRegion &other) const;

		bool operator<(const TextureRegion &other) const;

		/*!
		 * Represents the whole texture.
		 */
		const static TextureRegion BASE;
	};

	class Texture
	{
	private:
		uint32_t _id;
		uint32_t _width;
		uint32_t _height;
		uint32_t _channels;

	public:
		Texture(uint32_t id, uint32_t width, uint32_t height, uint32_t channels);

		uint32_t getId() const;

		uint32_t getWidth() const;

		uint32_t getHeight() const;

		uint32_t getChannels() const;

		TextureRegion getRegion(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const;

		/*!
		 * Binds the texture at the specified texture unit. Default texture unit is {@code GL_TEXTURE0}.
		 * @param textureUnit The texture unit to bind.
		 */
		void bind(uint32_t textureUnit = GL_TEXTURE0) const;

		void unbind() const;

		bool operator==(const Texture &other) const;

		bool operator<(const Texture &other) const;

		explicit operator bool() const;
	};

	namespace texture
	{
		extern std::optional<Texture> IONICENGINE_API
		load(const lambdacommon::ResourceName &name, const std::string &extension = "png",
			 TextureWrapMode wrapMode = CLAMP, TextureFilterMode filterMode = NEAREST_MIPMAP_LINEAR,
			 bool useMipmap = true);

		extern Texture IONICENGINE_API
		create(const lambdacommon::ResourceName &name, unsigned char image[], uint32_t width, uint32_t height,
			   uint32_t channels = 4, TextureWrapMode wrapMode = CLAMP,
			   TextureFilterMode filterMode = NEAREST_MIPMAP_LINEAR, bool useMipmap = true);

		extern Texture IONICENGINE_API getTexture(const lambdacommon::ResourceName &name);

		extern bool IONICENGINE_API hasTexture(const lambdacommon::ResourceName &name);

		extern TextureRegion
		newTextureRegion(uint32_t textureWidth, uint32_t textureHeight, uint32_t x, uint32_t y, uint32_t width,
						 uint32_t height);

		extern void IONICENGINE_API unbind();
	}
}

#endif //IONICENGINE_TEXTURES_H
