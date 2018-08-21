/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/textures.h"
#include <SOIL/SOIL.h>
#include <stdexcept>

namespace ionicengine
{
	Texture IONICENGINE_API createTexture(unsigned char image[], uint32_t width, uint32_t height, uint32_t channels)
	{
		return createTexture(image, width, height, channels, CLAMP, NEAREST);
	}

	Texture IONICENGINE_API createTexture(unsigned char image[], uint32_t width, uint32_t height, uint32_t channels,
	                                   TextureWrapMode wrapMode, TextureFilterMode filterMode)
	{
		Texture texture{};
		texture.width = width;
		texture.height = height;
		texture.channels = channels;
		texture.id = SOIL_create_OGL_texture(image, width, height, channels, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);
		if (!texture.id)
			throw std::runtime_error(std::string("Failed to load texture\n").append(SOIL_last_result()));

		//GL CALLS

		return texture;
	}
}