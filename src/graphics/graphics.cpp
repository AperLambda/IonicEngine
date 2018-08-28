/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/graphics.h"
#include <utility>

using namespace lambdacommon;

#define ENABLE_OPENGL_OPTIONS glEnable(GL_CULL_FACE); \
                                glEnable(GL_BLEND); \
                                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#define DISABLE_OPENGL_OPTIONS glDisable(GL_CULL_FACE); \
                                glDisable(GL_BLEND);

namespace ionicengine
{
	Graphics::Graphics(const std::pair<uint32_t, uint32_t> &framebufferSize)
			: _framebufferSize(framebufferSize), _projection2d(
			glm::ortho(0.0f, static_cast<float>(framebufferSize.first), static_cast<float>(framebufferSize.second),
					   0.0f))
	{}

	std::pair<uint32_t, uint32_t> Graphics::getFramebufferSize() const
	{
		return _framebufferSize;
	}

	uint32_t Graphics::getWidth() const
	{
		return _framebufferSize.first;
	}

	uint32_t Graphics::getHeight() const
	{
		return _framebufferSize.second;
	}

	glm::mat4 Graphics::getOrthoProjection() const
	{
		return _projection2d;
	}

	void Graphics::resetTransform()
	{
		_transform = glm::mat4{1.0f};
	}

	void Graphics::translate(float x, float y, float z)
	{
		translate(glm::vec3(x, y, z));
	}

	void Graphics::translate(const glm::vec3 &translateVector)
	{
		_transform = glm::translate(_transform, translateVector);
	}

	void Graphics::rotate(float radians, float xAxis, float yAxis, float zAxis)
	{
		rotate(radians, glm::vec3(xAxis, yAxis, zAxis));
	}

	void Graphics::rotate(float radians, const glm::vec3 &axis)
	{
		_transform = glm::rotate(_transform, radians, axis);
	}

	void Graphics::scale(float xScale, float yScale, float zScale)
	{
		scale(glm::vec3(xScale, yScale, zScale));
	}

	void Graphics::scale(const glm::vec3 &scale)
	{
		_transform = glm::scale(_transform, scale);
	}

	void Graphics::drawImage(const lambdacommon::ResourceName &texture, float x, float y, float width, float height,
							 const TextureRegion &region)
	{
		if (!texture::hasTexture(texture))
			return;
		drawImage(texture::getTexture(texture), x, y, width, height, region);
	}

	class GraphicsGL3 : public Graphics
	{
	private:
		uint32_t vao, vbo;
	public:
		explicit GraphicsGL3(const std::pair<uint32_t, uint32_t> &framebufferSize, uint32_t vao, uint32_t vbo)
				: Graphics(framebufferSize), vao(vao), vbo(vbo)
		{
		}

		void setColor(const lambdacommon::Color &color) override
		{
			this->color = color;
		}

		void drawLine2D(float x, float y, float x2, float y2) override
		{
			if (!shader::hasShader(IONICENGINE_SHADERS_2DBASIC))
				return;

			// Set OpenGL options.
			ENABLE_OPENGL_OPTIONS;

			GLfloat vertices[2][4] = {
					{x,         y, 0.f, 0.f},
					{x2, y2,          0.f, 0.f}
			};

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_2DBASIC);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("transform", _transform);
			shader.setColor(color);

			glBindVertexArray(this->vao);

			// Update content of VBO memory.
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad.
			glDrawArrays(GL_LINES, 0, 2);

			glBindVertexArray(0);

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void drawQuad(float x, float y, float width, float height) override
		{
			if (!shader::hasShader(IONICENGINE_SHADERS_2DBASIC))
				return;

			// Set OpenGL options.
			ENABLE_OPENGL_OPTIONS;

			GLfloat vertices[6][4] = {
					{x,         y + height, 0.f, 0.f},
					{x + width, y,          0.f, 0.f},
					{x,         y,          0.f, 0.f},
					{x,         y + height, 0.f, 0.f},
					{x + width, y + height, 0.f, 0.f},
					{x + width, y,          0.f,0.f}
			};

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_2DBASIC);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("transform", _transform);
			shader.setColor(color);

			glBindVertexArray(this->vao);

			// Update content of VBO memory.
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad.
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void drawImage(const Texture &texture, float x, float y, float width, float height,
					   const TextureRegion &region) override
		{
			if (!texture)
				return;
			if (!shader::hasShader(IONICENGINE_SHADERS_IMAGE))
				return;

			// Set OpenGL options.
			ENABLE_OPENGL_OPTIONS;

			GLfloat vertices[6][4] = {
					{x,         y + height, region.minX(), region.maxY()},
					{x + width, y,          region.maxX(), region.minY()},
					{x,         y,          region.minX(), region.minY()},
					{x,         y + height, region.minX(), region.maxY()},
					{x + width, y + height, region.maxX(), region.maxY()},
					{x + width, y,          region.maxX(), region.minY()}
			};

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_IMAGE);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("transform", _transform);
			shader.setInteger("image", 0);
			shader.setColor(color);

			texture.bind();

			glBindVertexArray(this->vao);

			// Update content of VBO memory.
			glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			glBindBuffer(GL_ARRAY_BUFFER, 0);
			// Render quad.
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glBindVertexArray(0);

			texture.unbind();

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void drawText(const Font &font, float x, float y, const std::string &text, float scale) override
		{
			if (!shader::hasShader(SHADER_TEXT))
				return;
			// Set OpenGL options
			ENABLE_OPENGL_OPTIONS;

			// Shader
			auto shader = shader::getShader(SHADER_TEXT);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("transform", _transform);
			shader.setInteger("text", 0);
			shader.setColor(color);
			glActiveTexture(GL_TEXTURE0);
			glBindVertexArray(this->vao);

			float originalX = x;
			float originalY = y;

			// Iterate through all characters.
			std::string::const_iterator c;
			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = font.getCharacter(*c);

				// Special characters handling.
				if (*c == '\t')
				{
					for (uint32_t i = 0; i < font.getTabSize(); i++)
						x += (ch.advance >> 6) * scale;
					continue;
				}
				else if (*c == '\n')
				{
					x = originalX;
					y += font.getHeight() * scale;
					continue;
				}

				GLfloat xpos = x + ch.bearing.x * scale;
				GLfloat ypos = y + (font.getCharacter('H').bearing.y - ch.bearing.y) * scale;

				GLfloat w = ch.size.x * scale;
				GLfloat h = ch.size.y * scale;
				// Update VBO for each character.
				GLfloat vertices[6][4] = {
						{xpos,     ypos + h, 0.0, 1.0},
						{xpos + w, ypos,     1.0, 0.0},
						{xpos,     ypos,     0.0, 0.0},

						{xpos,     ypos + h, 0.0, 1.0},
						{xpos + w, ypos + h, 1.0, 1.0},
						{xpos + w, ypos,     1.0, 0.0}
				};
				// Render glyph texture over quad.
				glBindTexture(GL_TEXTURE_2D, ch.textureId);
				// Update content of VBO memory.
				glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
				glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

				glBindBuffer(GL_ARRAY_BUFFER, 0);
				// Render quad.
				glDrawArrays(GL_TRIANGLES, 0, 6);
				// Now advance cursors for next glyph.
				x += (ch.advance >> 6) * scale;
			}
			glBindVertexArray(0);
			texture::unbind();

			// Unset OpenGL options
			DISABLE_OPENGL_OPTIONS;
		}
	};

	GraphicsManager::GraphicsManager() : _graphicsUsed(GRAPHICS_GL3)
	{}

	void GraphicsManager::init()
	{
		//registerGraphics(GRAPHICS_GL1, [](const glm::mat4 &projection) { return (Graphics *) new GraphicsGL1(projection); });
		registerGraphics(GRAPHICS_GL3,
						 [this](const std::pair<uint32_t, uint32_t> &framebufferSize)
						 {
							 return (Graphics *) new GraphicsGL3(framebufferSize, vao, vbo);
						 });
		_graphicsUsed = GRAPHICS_GL3;

		// Configure VAO/VBO for texture quads
		glGenVertexArrays(1, &this->vao);
		glGenBuffers(1, &this->vbo);
		glBindVertexArray(this->vao);
		glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		if (!shader::compile(IONICENGINE_SHADERS_2DBASIC))
			throw std::runtime_error("Cannot load 2d basic shaders.");
		if (!shader::compile(IONICENGINE_SHADERS_IMAGE))
			throw std::runtime_error("Cannot load image shaders.");
		if (!shader::compile(SHADER_TEXT))
			throw std::runtime_error("Cannot load text shaders.");

		// Configure Framebuffer
		glGenFramebuffers(1, &framebuffer);
	}

	void GraphicsManager::registerGraphics(ResourceName name, const newGraphicsFunction &newGraphics)
	{
		if (_graphics.count(name))
			throw std::invalid_argument(
					"GraphicsManager has already registered the graphics with name '" + name.toString() + "'!");
		_graphics[name] = newGraphics;
	}

	void GraphicsManager::useGraphics(ResourceName name)
	{
		_graphicsUsed = std::move(name);
	}

	Graphics *GraphicsManager::newGraphics(std::pair<uint32_t, uint32_t> framebufferSize) const
	{
		if (!_graphics.count(_graphicsUsed))
			return nullptr;
		return _graphics.at(_graphicsUsed)(framebufferSize);
	}

	const GraphicsManager *graphicsManager = new GraphicsManager{};

	GraphicsManager *getGraphicsManager()
	{
		return const_cast<GraphicsManager *>(graphicsManager);
	}
}