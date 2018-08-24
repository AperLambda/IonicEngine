/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include <utility>

using namespace lambdacommon;

namespace ionicengine
{
	Graphics::Graphics(const glm::mat4 &projection2d) : _projection2d(projection2d)
	{}

	glm::mat4 Graphics::getOrthoProjection() const
	{
		return _projection2d;
	}

	class GraphicsGL1 : public Graphics
	{
	public:
		explicit GraphicsGL1(const glm::mat4 &projection2d) : Graphics(projection2d)
		{}

		void setColor(const lambdacommon::Color &color) override
		{
			this->color = color;
			glColor4f(color.red(), color.green(), color.blue(), color.alpha());
		}

		void use2D()
		{
		};

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

	class GraphicsGL3 : public Graphics
	{
	private:
		uint32_t vao, vbo;
	public:
		explicit GraphicsGL3(const glm::mat4 &projection2d, uint32_t vao, uint32_t vbo) : Graphics(projection2d),
																						  vao(vao), vbo(vbo)
		{
		}

		void setColor(const lambdacommon::Color &color) override
		{
			this->color = color;
		}

		void drawLine2D(float x, float y, float x2, float y2) override
		{

		}

		void drawQuad() override
		{

		}

		void drawImage(Texture *texture, float x, float y, float width, float height) override
		{

		}

		void drawText(const Font &font, float x, float y, const std::string &text, float scale) override
		{
			if (!shader::hasShader(SHADER_TEXT))
				return;
			// Set OpenGL options
			glEnable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			// Shader
			auto shader = shader::getShader(SHADER_TEXT);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
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
					y += font.getHeight();
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
		}
	};

	GraphicsManager::GraphicsManager() : _graphicsUsed(GRAPHICS_GL3)
	{}

	void GraphicsManager::init()
	{
		//registerGraphics(GRAPHICS_GL1, [](const glm::mat4 &projection) { return (Graphics *) new GraphicsGL1(projection); });
		registerGraphics(GRAPHICS_GL3,
						 [this](const glm::mat4 &projection) { return (Graphics *) new GraphicsGL3(projection, vao, vbo); });
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
		if (!shader::compile(SHADER_TEXT))
			throw std::runtime_error("Cannot load text shaders.");
	}

	void GraphicsManager::registerGraphics(ResourceName name, newGraphicsFunction newGraphics)
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
		auto orthoProjection = glm::ortho(0.0f, static_cast<float>(framebufferSize.first),
										  static_cast<float>(framebufferSize.second), 0.0f);
		if (!_graphics.count(_graphicsUsed))
			return nullptr;
		return _graphics.at(_graphicsUsed)(orthoProjection);
	}

	const GraphicsManager *graphicsManager = new GraphicsManager{};

	GraphicsManager *getGraphicsManager()
	{
		return const_cast<GraphicsManager *>(graphicsManager);
	}
}