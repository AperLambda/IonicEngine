/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <utility>
#include "../../include/ionicengine/graphics/graphics.h"

using namespace lambdacommon;

namespace ionicengine
{
	class GraphicsGL1 : public Graphics
	{
	public:
		void drawLine2D(float x, float y, float x2, float y2) override
		{
			glBegin(GL_LINE);
			glVertex2f(x, y);
			glVertex2f(x2, y2);
			glEnd();
		}

		void drawQuad() override
		{

		}

		void drawImage(Texture *texture, float x, float y, float width, float height) override
		{

		}
	};

	void GraphicsManager::init()
	{
		registerGraphics(GRAPHICS_GL1, []()
			{ return (Graphics *) new GraphicsGL1(); });
		_graphicsUsed = GRAPHICS_GL1;
	}

	std::string resNameToString(const ResourceName &name)
	{
		return name.getDomain() + ":" + name.getName();
	}

	void GraphicsManager::registerGraphics(ResourceName name, newGraphicsFunction newGraphics)
	{
		if (_graphics.count(name))
			throw std::invalid_argument("GraphicsManager already has registered the graphics with name '" + name.getDomain() + ":" + name.getName() + "'!");
		_graphics[name] = newGraphics;
	}

	void GraphicsManager::useGraphics(ResourceName name)
	{
		_graphicsUsed = std::move(name);
	}

	Graphics *GraphicsManager::newGraphics()
	{
		if (!_graphics.count(_graphicsUsed))
			return nullptr;
		return  _graphics[_graphicsUsed]();
	}
}