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
#include "utils.h"
#include <lambdacommon/graphics/color.h>
#include <lambdacommon/maths.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <map>
#include <functional>

namespace ionicengine
{
	static uint32_t quadVAO = 0, quadVBO = 0,
			quadOutlineVAO = 0, quadOutlineVBO = 0,
			textureVAO = 0, textureVBO = 0;

	class IONICENGINE_API Graphics
	{
	protected:
		lambdacommon::Color color = lambdacommon::Color::COLOR_WHITE;
		std::pair<uint32_t, uint32_t> _framebufferSize;
		glm::mat4 _projection2d;
		glm::mat4 _transform{1.0f};

	public:
		Graphics(const std::pair<uint32_t, uint32_t> &framebufferSize);

		virtual ~Graphics();

		std::pair<uint32_t, uint32_t> getFramebufferSize() const;

		/*!
		 * Gets the width of the frame buffer in pixels.
		 * @return The width of the frame buffer.
		 */
		uint32_t getWidth() const;

		/*!
		 * Gets the height of the frame buffer in pixels.
		 * @return The height of the frame buffer.
		 */
		uint32_t getHeight() const;

		/*!
		 * Gets the width of the frame buffer in pixels.
		 * @return The width of the frame buffer.
		 */
		float getFloatingWidth() const;

		/*!
		 * Gets the height of the frame buffer in pixels.
		 * @return The height of the frame buffer.
		 */
		float getFloatingHeight() const;

		/*!
		 * Gets the matrix of the orthographic projection.
		 * @return The orthographic projection.
		 */
		glm::mat4 getOrthoProjection() const;

		/*!
		 * Updates the graphics with a new width and height.
		 * @param width Width.
		 * @param height Height.
		 */
		void updateFramebufferSize(uint32_t width, uint32_t height);

		/*!
		 * Sets the color of the objects to draw.
		 * @param color The color to use.
		 */
		virtual void setColor(const lambdacommon::Color &color) = 0;

		/*!
		 * Resets the transformation matrix.
		 */
		void resetTransform();

		void translate(float x, float y, float z = 0.0f);

		void translate(const glm::vec3 &translateVector);

		void rotate(float radians, float xAxis, float yAxis, float zAxis);

		void rotate(float radians, const glm::vec3 &axis);

		void rotate2DCentered(float radians, uint32_t width, uint32_t height);

		void scale(float xScale, float yScale, float zScale = 1.0f);

		void scale(const glm::vec3 &scale);

		virtual void drawLine2D(int x, int y, int x2, int y2) = 0;

		virtual void drawQuad(int x, int y, uint32_t width, uint32_t height) = 0;

		virtual void drawQuadOutline(int x, int y, uint32_t width, uint32_t height) = 0;

		virtual void
		drawImage(const lambdacommon::ResourceName &texture, int x, int y, uint32_t width, uint32_t height,
				  const TextureRegion &region = TextureRegion::BASE);

		virtual void drawImage(const Texture &texture, int x, int y, uint32_t width, uint32_t height,
							   const TextureRegion &region = TextureRegion::BASE) = 0;

		/*!
		 * Draws text with the specified font, at the specified position.
		 * @param font The font to use.
		 * @param x The x position of the text.
		 * @param y The y position of the text.
		 * @param text The text to draw.
		 * @param scale The scale of the text.
		 */
		virtual void drawText(const Font &font, float x, float y, const std::string &text, float maxWidth = 0.f,
							  float maxHeight = 0.f, float scale = 1.0) = 0;
	};

	//typedef Graphics* (*newGraphicsFunction)(const glm::mat4 &projection);
	typedef std::function<Graphics *(const std::pair<uint32_t, uint32_t> &framebufferSize)> newGraphicsFunction;

	class IONICENGINE_API GraphicsManager
	{
	private:
		std::map<lambdacommon::ResourceName, newGraphicsFunction> _graphics;
		lambdacommon::ResourceName _graphicsUsed;
		uint32_t vao = 0, vbo = 0, framebuffer = 0, textureColorBuffer = 0, rbo = 0;

	public:
		GraphicsManager();

		void init();

		void registerGraphics(lambdacommon::ResourceName name, const newGraphicsFunction &newGraphics);

		void useGraphics(lambdacommon::ResourceName name);

		Graphics *newGraphics(std::pair<uint32_t, uint32_t> framebufferSize) const;
	};

	extern GraphicsManager *IONICENGINE_API getGraphicsManager();
}

#endif //IONICENGINE_GRAPHICS_H
