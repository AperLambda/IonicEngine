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

#define toFloat(i) static_cast<float>(i)
#define clampX(i) toFloat(lambdacommon::maths::clamp(i, 0.f, static_cast<float>(getWidth())))
#define clampY(i) toFloat(lambdacommon::maths::clamp(i, 0.f, static_cast<float>(getHeight())))

namespace ionicengine
{
	Graphics::Graphics(const std::pair<uint32_t, uint32_t> &framebufferSize)
			: _framebufferSize(framebufferSize), _projection2d(
			glm::ortho(0.0f, static_cast<float>(framebufferSize.first), static_cast<float>(framebufferSize.second),
					   0.0f))
	{}

	Graphics::~Graphics() = default;

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

	float Graphics::getFloatingWidth() const
	{
		return static_cast<float>(this->getWidth());
	}

	float Graphics::getFloatingHeight() const
	{
		return static_cast<float>(this->getHeight());
	}

	glm::mat4 Graphics::getOrthoProjection() const
	{
		return _projection2d;
	}

	void Graphics::updateFramebufferSize(uint32_t width, uint32_t height)
	{
		_framebufferSize = {width, height};
		_projection2d = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
	}

	void Graphics::resetTransform()
	{
		_transform = glm::mat4{1.0f};
	}

	void Graphics::translate(int x, int y, int z)
	{
		translate(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
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

	void Graphics::rotate2DCentered(float radians, uint32_t width, uint32_t height)
	{
		float x = width / 2.f;
		float y = height / 2.f;

		this->translate(x, y);
		this->rotate(radians, 0.f, 0.f, 1.f);
		this->translate(-x, -y);
	}

	void Graphics::scale(float xScale, float yScale, float zScale)
	{
		scale(glm::vec3(xScale, yScale, zScale));
	}

	void Graphics::scale(const glm::vec3 &scale)
	{
		_transform = glm::scale(_transform, scale);
	}

	void Graphics::drawLine2D(int x, int y, int x2, int y2)
	{
		drawLine2D(static_cast<float>(x), static_cast<float>(y), static_cast<float>(x2), static_cast<float>(y2));
	}

	void Graphics::drawQuad(int x, int y, uint32_t width, uint32_t height)
	{
		drawQuad(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
	}

	void Graphics::drawQuadOutline(int x, int y, uint32_t width, uint32_t height)
	{
		drawQuadOutline(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
						static_cast<float>(height));
	}

	void Graphics::drawImage(const lambdacommon::ResourceName &texture, int x, int y, uint32_t width, uint32_t height,
							 const TextureRegion &region)
	{
		drawImage(texture, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
				  static_cast<float>(height), region);
	}

	void Graphics::drawImage(const lambdacommon::ResourceName &texture, float x, float y, float width, float height,
							 const TextureRegion &region)
	{
		if (!texture::hasTexture(texture))
			return;
		drawImage(texture::getTexture(texture), x, y, width, height, region);
	}

	void Graphics::drawImage(const Texture &texture, int x, int y, uint32_t width, uint32_t height,
							 const TextureRegion &region)
	{
		drawImage(texture, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height), region);
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

			glm::mat4 model{1.f};

			GLfloat vertices[2][4] = {
					{clampX(x),  clampY(y),  0.f, 0.f},
					{clampX(x2), clampY(y2), 0.f, 0.f}
			};

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_2DBASIC);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("model", model);
			shader.setMatrix4f("transform", _transform);
			shader.setColor(color);

			vao::bind(this->vao);

			// Update content of VBO memory.
			vbo::bind(this->vbo);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

			vbo::bind(0);
			// Render quad.
			glDrawArrays(GL_LINES, 0, 2);

			vao::unbind();

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void drawQuad(float x, float y, float width, float height) override
		{
			if (!shader::hasShader(IONICENGINE_SHADERS_2DBASIC))
				return;

			// Set OpenGL options.
			ENABLE_OPENGL_OPTIONS;

			glm::mat4 model{1.f};
			model = glm::translate(model, {x, y, 0.f});
			model = glm::scale(model, {width, height, 1.f});

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_2DBASIC);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("model", model);
			shader.setMatrix4f("transform", _transform);
			shader.setColor(color);

			vao::bind(quadVAO);
			// Render quad.
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			vao::unbind();

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void drawQuadOutline(float x, float y, float width, float height) override
		{
			if (!shader::hasShader(IONICENGINE_SHADERS_2DBASIC))
				return;

			// Set OpenGL options.
			ENABLE_OPENGL_OPTIONS;

			glm::mat4 model{1.f};
			model = glm::translate(model, {x, y, 0.f});
			model = glm::scale(model, {width, height, 1.f});

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_2DBASIC);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("model", model);
			shader.setMatrix4f("transform", _transform);
			shader.setColor(color);

			vao::bind(quadOutlineVAO);
			// Render quad.
			glDrawArrays(GL_LINE_LOOP, 0, 4);
			vao::unbind();

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

			glm::mat4 model{1.f};
			model = glm::translate(model, {x, y, 0.f});
			model = glm::scale(model, {width, height, 1.f});

			GLfloat texCoords[4][2] = {
					{region.minX(), region.maxY()},
					{region.maxX(), region.maxY()},
					{region.minX(), region.minY()},
					{region.maxX(), region.minY()}
			};

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_IMAGE);
			shader.use();
			shader.setMatrix4f("projection", _projection2d);
			shader.setMatrix4f("model", model);
			shader.setMatrix4f("transform", _transform);
			for (size_t i = 0; i < 4; i++)
			{
				shader.setVector2f("texCoordsData[" + std::to_string(i) + "]", texCoords[i][0], texCoords[i][1]);
			}
			shader.setInteger("image", 0);
			shader.setColor(color);

			texture.bind();

			vao::bind(textureVAO);
			// Render quad.
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			vao::unbind();

			texture.unbind();

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void
		drawText(const Font &font, int xPos, int yPos, const std::string &text, uint32_t maxWidth, uint32_t maxHeight,
				 float scale) override
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
			vao::bind(this->vao);

			glm::mat4 model{1.f};

			auto x = static_cast<float>(xPos);
			auto y = static_cast<float>(yPos);
			float originalX = x;
			float originalY = y;

			// Iterate through all characters.
			std::string::const_iterator c;
			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = font.getCharacter(*c);

				if (maxWidth != 0.f && x + (ch.advance >> 6) >= (originalX + maxWidth))
				{
					x = originalX;
					y += font.getHeight() * scale;
				}
				if (maxHeight != 0.f && (y + font.getHeight() / 2.f) > (originalY + maxHeight))
					break;

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

				// Define model position and scale.
				model = glm::mat4(1.f);
				GLfloat xpos = x + ch.bearing.x * scale;
				GLfloat ypos = y + (font.getCharacter('H').bearing.y - ch.bearing.y) * scale;
				model = glm::translate(model, {xpos, ypos, 0.f});
				GLfloat w = ch.size.x * scale;
				GLfloat h = ch.size.y * scale;
				model = glm::scale(model, {w, h, 0.f});

				shader.setMatrix4f("model", model);
				// Render glyph texture over quad.
				glBindTexture(GL_TEXTURE_2D, ch.textureId);
				// Render quad.
				glDrawArrays(GL_TRIANGLES, 0, 6);
				// Now advance cursors for next glyph.
				x += (ch.advance >> 6) * scale;
			}
			vao::unbind();
			texture::unbind();

			// Unset OpenGL options
			DISABLE_OPENGL_OPTIONS;
		}
	};

	void shapeQuadInit()
	{
		/*GLfloat outlineVertices[] = {0.f, 1.f,
									 0.f, 0.f,
									 1.f, 0.f,
									 1.f, 1.f};*/
		GLfloat outlineVertices[] = {0.f, 0.f,
									 1.f, 0.f,
									 1.f, 1.f,
									 0.f, 1.f};

		GLfloat fillVertices[] = {0.f, 0.f,
								  0.f, 1.f,
								  1.f, 0.f,
								  1.f, 1.f};

		glGenVertexArrays(1, &quadVAO);
		glGenBuffers(1, &quadVBO);

		vbo::bind(quadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fillVertices), fillVertices, GL_STATIC_DRAW);

		vao::bind(quadVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();

		glGenVertexArrays(1, &quadOutlineVAO);
		glGenBuffers(1, &quadOutlineVBO);

		vbo::bind(quadOutlineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(outlineVertices), outlineVertices, GL_STATIC_DRAW);

		vao::bind(quadOutlineVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();
	}

	void shapeTextureInit()
	{
		// Defines vertices, z is texture coordinates index.
		GLfloat vertices[4][3] = {
				{0.f, 1.f, 0},
				{1.f, 1.f, 1},
				{0.f, 0.f, 2},
				{1.f, 0.f, 3}
		};

		glGenVertexArrays(1, &textureVAO);
		glGenBuffers(1, &textureVBO);

		vbo::bind(textureVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		vao::bind(textureVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();
	}

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
		vao::bind(this->vao);
		vbo::bind(this->vbo);
		GLfloat textVertices[6][4] = {
				{0.f, 1.f, 0.0, 1.0},
				{1.f, 0.f, 1.0, 0.0},
				{0.f, 0.f, 0.0, 0.0},

				{0.f, 1.f, 0.0, 1.0},
				{1.f, 1.f, 1.0, 1.0},
				{1.f, 0.f, 1.0, 0.0}
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, textVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();

		shapeQuadInit();
		shapeTextureInit();

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