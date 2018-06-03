/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_GRAPHICS_H
#define IONICENGINE_GRAPHICS_H

#include <lambdacommon/resources.h>
#include "textures.h"

namespace ionicengine
{
	class Graphics
	{
	public:
		virtual void drawLine2D(float x, float y, float x2, float y2) = 0;

		virtual void drawQuad() = 0;

		virtual void drawImage(Texture *texture, float x, float y, float width, float height) = 0;
	};

	class GraphicsManager
	{
	public:
		void registerGraphics(lambdacommon::ResourceName name, Graphics* (*newGraphics)());
	};
}

#endif //IONICENGINE_GRAPHICS_H
