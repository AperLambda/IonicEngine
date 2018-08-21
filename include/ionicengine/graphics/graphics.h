/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_GRAPHICS_H
#define IONICENGINE_GRAPHICS_H

#include "textures.h"
#include <lambdacommon/resources.h>
#include <map>

namespace ionicengine
{
	const lambdacommon::ResourceName GRAPHICS_GL1{"ionicengine", "graphics/gl1"};
	const lambdacommon::ResourceName GRAPHICS_GL4{"ionicengine", "graphics/gl4"};

	class IONICENGINE_API Graphics
	{
	public:
		virtual void drawLine2D(float x, float y, float x2, float y2) = 0;

		virtual void drawQuad() = 0;

		virtual void drawImage(Texture *texture, float x, float y, float width, float height) = 0;
	};

	typedef Graphics* (*newGraphicsFunction)();

	class IONICENGINE_API GraphicsManager
	{
	private:
		std::map<lambdacommon::ResourceName, newGraphicsFunction> _graphics;
		lambdacommon::ResourceName _graphicsUsed;

	public:
		void init();

		void registerGraphics(lambdacommon::ResourceName name, newGraphicsFunction newGraphics);

		void useGraphics(lambdacommon::ResourceName name);

		Graphics *newGraphics();
	};
}

#endif //IONICENGINE_GRAPHICS_H
