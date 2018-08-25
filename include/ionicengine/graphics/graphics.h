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

#include "font.h"
#include "textures.h"
#include "shader.h"
#include <lambdacommon/graphics/color.h>
#include <glm/glm.hpp>
#include <map>
#include <functional>

namespace ionicengine
{
	class IONICENGINE_API Graphics
	{
	protected:
		lambdacommon::Color color = lambdacommon::Color::WHITE;
		glm::mat4 _projection2d;
		glm::mat4 _transform{1.0f};

	public:
		Graphics(const glm::mat4 &projection2d);

		glm::mat4 getOrthoProjection() const;

		virtual void setColor(const lambdacommon::Color &color) = 0;

		void resetTransform();

		void translate(float x, float y, float z = 0.0f);

		void translate(const glm::vec3 &translateVector);

		void rotate(float radians, float xAxis, float yAxis, float zAxis);

		void rotate(float radians, const glm::vec3 &axis);

		void scale(float xScale, float yScale, float zScale = 1.0f);

		void scale(const glm::vec3 &scale);

		virtual void drawLine2D(float x, float y, float x2, float y2) = 0;

		virtual void drawQuad() = 0;

		virtual void drawImage(Texture *texture, float x, float y, float width, float height) = 0;

		virtual void drawText(const Font &font, float x, float y, const std::string &text, float scale = 1.0) = 0;
	};

	//typedef Graphics* (*newGraphicsFunction)(const glm::mat4 &projection);
	typedef std::function<Graphics *(const glm::mat4 &projection)> newGraphicsFunction;

	class IONICENGINE_API GraphicsManager
	{
	private:
		std::map<lambdacommon::ResourceName, newGraphicsFunction> _graphics;
		lambdacommon::ResourceName _graphicsUsed;
		uint32_t vao = 0, vbo = 0;

	public:
		GraphicsManager();

		void init();

		void registerGraphics(lambdacommon::ResourceName name, newGraphicsFunction newGraphics);

		void useGraphics(lambdacommon::ResourceName name);

		Graphics *newGraphics(std::pair<uint32_t, uint32_t> framebufferSize) const;
	};

	extern GraphicsManager *IONICENGINE_API getGraphicsManager();
}

#endif //IONICENGINE_GRAPHICS_H
