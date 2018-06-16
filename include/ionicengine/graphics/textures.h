/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_TEXTURES_H
#define IONICENGINE_TEXTURES_H

#include "../ionicengine.h"

namespace ionicengine
{
	enum TextureWrapMode
	{
		CLAMP = 0x2900,
		CLAMP_TO_EDGE = 0x812F,
		REPEAT = 0x2901
	};

	enum TextureFilterMode
	{
		LINEAR = 0x2601,
		NEAREST = 0x2600
	};

	struct Texture
	{
		uint32_t id;
		uint32_t width;
		uint32_t height;
		uint32_t channels;
	};

	extern Texture IONICENGINE_API loadTexture(std::string path);

	extern Texture IONICENGINE_API
	loadTexture(std::string path, TextureWrapMode wrapMode, TextureFilterMode filterMode);

	extern Texture IONICENGINE_API
	createTexture(unsigned char image[], uint32_t width, uint32_t height, uint32_t channels);

	extern Texture IONICENGINE_API
	createTexture(unsigned char image[], uint32_t width, uint32_t height, uint32_t channels,
	              TextureWrapMode wrapMode, TextureFilterMode filterMode);
}

#endif //IONICENGINE_TEXTURES_H
