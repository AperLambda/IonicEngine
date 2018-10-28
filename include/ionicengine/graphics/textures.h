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
		float _min_x;

		/*!
		 * Stores the top-left texture Y coordinate, with a value between 0.0 and 1.0.
		 */
		float _min_y;

		/*!
		 * Stores the bottom-right texture X coordinate, with a value between 0.0 and 1.0.
		 */
		float _max_x;

		/*!
		 * Stores the bottom-right texture Y coordinate, with a value between 0.0 and 1.0.
		 */
		float _max_y;

	public:
		TextureRegion(float min_x, float min_y, float max_x, float max_y);

		/*!
		 * Gets the top-left corner texture X coordinate.
		 *
		 * @return X coordinate between 0.0 and 1.0.
		 */
		float min_x() const;

		/*!
		 * Gets the top-left corner texture Y coordinate.
		 *
		 * @return Y coordinate between 0.0 and 1.0.
		 */
		float min_y() const;

		/*!
		 * Gets the bottom-right corner texture X coordinate.
		 *
		 * @return X coordinate between 0.0 and 1.0.
		 */
		float max_x() const;

		/*!
		 * Gets the bottom-right corner texture Y coordinate.
		 *
		 * @return Y coordinate between 0.0 and 1.0.
		 */
		float max_y() const;

		/*!
		 * Sets the top-left corner texture X coordinate.
		 *
		 * @param min_x X coordinate between 0.0 and 1.0.
		 */
		void min_x(float min_x);

		/*!
		 * Sets the top-left corner texture Y coordinate.
		 *
		 * @param min_y Y coordinate between 0.0 and 1.0.
		 */
		void min_y(float min_y);

		/*!
		 * Sets the bottom-right corner texture X coordinate.
		 *
		 * @param max_x X coordinate between 0.0 and 1.0.
		 */
		void max_x(float max_x);

		/*!
		 * Sets the bottom-right corner texture Y coordinate.
		 *
		 * @param max_y Y coordinate between 0.0 and 1.0.
		 */
		void max_y(float max_y);

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

		uint32_t get_id() const;

		uint32_t get_width() const;

		uint32_t get_height() const;

		uint32_t get_channels() const;

		TextureRegion new_region(uint32_t x, uint32_t y, uint32_t width, uint32_t height) const;

		/*!
		 * Binds the texture at the specified texture unit. Default texture unit is {@code GL_TEXTURE0}.
		 * @param texture_unit The texture unit to bind.
		 */
		void bind(uint32_t texture_unit = GL_TEXTURE0) const;

		void unbind() const;

		void delete_texture();

		bool operator==(const Texture &other) const;

		bool operator<(const Texture &other) const;

		explicit operator bool() const;
	};

	namespace texture
	{
		extern std::optional<Texture> IONICENGINE_API
		load(const lambdacommon::ResourceName &name, const std::string &extension = "png",
			 TextureWrapMode wrap_mode = CLAMP, TextureFilterMode filter_mode = NEAREST_MIPMAP_LINEAR,
			 bool use_mipmap = true);

		extern Texture IONICENGINE_API
		create(const lambdacommon::ResourceName &name, unsigned char image[], uint32_t width, uint32_t height,
			   uint32_t channels = 4, TextureWrapMode wrap_mode = CLAMP,
			   TextureFilterMode filter_mode = NEAREST_MIPMAP_LINEAR, bool use_mipmap = true);

		extern Texture IONICENGINE_API get_texture(const lambdacommon::ResourceName &name);

		extern bool IONICENGINE_API has_texture(const lambdacommon::ResourceName &name);

		extern TextureRegion
		new_texture_region(uint32_t textureWidth, uint32_t textureHeight, uint32_t x, uint32_t y, uint32_t width,
						   uint32_t height);

		extern void IONICENGINE_API delete_texture(const lambdacommon::ResourceName &name);

		extern void IONICENGINE_API bind(uint32_t id);

		extern void IONICENGINE_API unbind();

		extern void IONICENGINE_API shutdown();
	}
}

#endif //IONICENGINE_TEXTURES_H
