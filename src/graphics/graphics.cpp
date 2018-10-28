/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/graphics.h"
#include "../../include/ionicengine/gl/buffer.h"
#include <utility>

using namespace lambdacommon;

#define ENABLE_OPENGL_OPTIONS glEnable(GL_CULL_FACE); \
                                glEnable(GL_BLEND); \
                                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#define DISABLE_OPENGL_OPTIONS glDisable(GL_CULL_FACE); \
                                glDisable(GL_BLEND);

#define toFloat(i) static_cast<float>(i)
#define clampX(i) toFloat(lambdacommon::maths::clamp(i, 0.f, static_cast<float>(get_width())))
#define clampY(i) toFloat(lambdacommon::maths::clamp(i, 0.f, static_cast<float>(get_height())))

namespace ionicengine
{
	Graphics::Graphics(const Dimension2D_u32 &framebufferSize)
			: _framebuffer_size(framebufferSize), _projection2d(
			glm::ortho(0.0f, static_cast<float>(framebufferSize.get_width()),
					   static_cast<float>(framebufferSize.get_height()),
					   0.0f))
	{}

	Graphics::~Graphics() = default;

	Dimension2D_u32 Graphics::get_framebuffer_size() const
	{
		return _framebuffer_size;
	}

	uint32_t Graphics::get_width() const
	{
		return _framebuffer_size.get_width();
	}

	uint32_t Graphics::get_height() const
	{
		return _framebuffer_size.get_height();
	}

	float Graphics::get_floating_width() const
	{
		return static_cast<float>(this->get_width());
	}

	float Graphics::get_floating_height() const
	{
		return static_cast<float>(this->get_height());
	}

	glm::mat4 Graphics::get_ortho_projection() const
	{
		return _projection2d;
	}

	void Graphics::update_framebuffer_size(uint32_t width, uint32_t height)
	{
		_framebuffer_size = {width, height};
		_projection2d = glm::ortho(0.0f, static_cast<float>(width), static_cast<float>(height), 0.0f);
	}

	void Graphics::reset_transform()
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

	void Graphics::rotate_2d_centered(float radians, uint32_t width, uint32_t height)
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

	void Graphics::draw_line_2d(int x, int y, int x2, int y2)
	{
		draw_line_2d(static_cast<float>(x), static_cast<float>(y), static_cast<float>(x2), static_cast<float>(y2));
	}

	void Graphics::draw_quad(int x, int y, uint32_t width, uint32_t height)
	{
		draw_quad(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width), static_cast<float>(height));
	}

	void Graphics::draw_quad_outline(int x, int y, uint32_t width, uint32_t height)
	{
		draw_quad_outline(static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
						  static_cast<float>(height));
	}

	void Graphics::draw_image(const lambdacommon::ResourceName &texture, int x, int y, uint32_t width, uint32_t height,
							  const TextureRegion &region)
	{
		draw_image(texture, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
				   static_cast<float>(height), region);
	}

	void Graphics::draw_image(const lambdacommon::ResourceName &texture, float x, float y, float width, float height,
							  const TextureRegion &region)
	{
		if (!texture::has_texture(texture))
			return;
		draw_image(texture::get_texture(texture), x, y, width, height, region);
	}

	void Graphics::draw_image(const Texture &texture, int x, int y, uint32_t width, uint32_t height,
							  const TextureRegion &region)
	{
		draw_image(texture, static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
				   static_cast<float>(height), region);
	}

	class GraphicsGL3 : public Graphics
	{
	private:
		uint32_t vao, vbo;
	public:
		explicit GraphicsGL3(const Dimension2D_u32 &framebuffer_size, uint32_t vao, uint32_t vbo)
				: Graphics(framebuffer_size), vao(vao), vbo(vbo)
		{
		}

		void set_color(const lambdacommon::Color &color) override
		{
			this->color = color;
		}

		void draw_line_2d(float x, float y, float x2, float y2) override
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
			shader.set_matrix_4f("projection", _projection2d);
			shader.set_matrix_4f("model", model);
			shader.set_matrix_4f("transform", _transform);
			shader.set_color(color);

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

		void draw_quad(float x, float y, float width, float height) override
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
			shader.set_matrix_4f("projection", _projection2d);
			shader.set_matrix_4f("model", model);
			shader.set_matrix_4f("transform", _transform);
			shader.set_color(color);

			vao::bind(quad_vao);
			// Render quad.
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			vao::unbind();

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void draw_quad_outline(float x, float y, float width, float height) override
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
			shader.set_matrix_4f("projection", _projection2d);
			shader.set_matrix_4f("model", model);
			shader.set_matrix_4f("transform", _transform);
			shader.set_color(color);

			vao::bind(quad_outline_vao);
			// Render quad.
			glDrawArrays(GL_LINE_LOOP, 0, 4);
			vao::unbind();

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void draw_image(const Texture &texture, float x, float y, float width, float height,
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
					{region.min_x(), region.max_y()},
					{region.max_x(), region.max_y()},
					{region.min_x(), region.min_y()},
					{region.max_x(), region.min_y()}
			};

			// Shader
			auto shader = shader::getShader(IONICENGINE_SHADERS_IMAGE);
			shader.use();
			shader.set_matrix_4f("projection", _projection2d);
			shader.set_matrix_4f("model", model);
			shader.set_matrix_4f("transform", _transform);
			for (size_t i = 0; i < 4; i++)
			{
				shader.set_vector_2f("texCoordsData[" + std::to_string(i) + "]", texCoords[i][0], texCoords[i][1]);
			}
			shader.set_integer("image", 0);
			shader.set_color(color);

			texture.bind();

			vao::bind(texture_vao);
			// Render quad.
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			vao::unbind();

			texture.unbind();

			// Unset OpenGL options.
			DISABLE_OPENGL_OPTIONS;
		}

		void
		draw_text(const Font &font, int xPos, int yPos, const std::string &text, uint32_t maxWidth, uint32_t maxHeight,
				  float scale) override
		{
			if (!shader::hasShader(SHADER_TEXT))
				return;
			// Set OpenGL options
			ENABLE_OPENGL_OPTIONS;

			// Shader
			auto shader = shader::getShader(SHADER_TEXT);
			shader.use();
			shader.set_matrix_4f("projection", _projection2d);
			shader.set_matrix_4f("transform", _transform);
			shader.set_integer("text", 0);
			shader.set_color(color);
			glActiveTexture(GL_TEXTURE0);
			vao::bind(this->vao);

			glm::mat4 model{1.f};

			auto x = static_cast<float>(xPos);
			auto y = static_cast<float>(yPos);
			float original_x = x;
			float original_y = y;

			// Iterate through all characters.
			std::string::const_iterator c;
			for (c = text.begin(); c != text.end(); c++)
			{
				Character ch = font.get_character(*c);

				if (maxWidth != 0.f && x + (ch.advance >> 6) >= (original_x + maxWidth))
				{
					x = original_x;
					y += font.get_height() * scale;
				}
				if (maxHeight != 0.f && (y + font.get_height() / 2.f) > (original_y + maxHeight))
					break;

				// Special characters handling.
				if (*c == '\t')
				{
					for (uint32_t i = 0; i < font.get_tab_size(); i++)
						x += (ch.advance >> 6) * scale;
					continue;
				}
				else if (*c == '\n')
				{
					x = original_x;
					y += font.get_height() * scale;
					continue;
				}

				// Define model position and scale.
				model = glm::mat4(1.f);
				GLfloat xpos = x + ch.bearing.x * scale;
				GLfloat ypos = y + (font.get_character('H').bearing.y - ch.bearing.y) * scale;
				model = glm::translate(model, {xpos, ypos, 0.f});
				GLfloat w = ch.size.x * scale;
				GLfloat h = ch.size.y * scale;
				model = glm::scale(model, {w, h, 0.f});

				shader.set_matrix_4f("model", model);
				// Render glyph texture over quad.
				glBindTexture(GL_TEXTURE_2D, ch.texture_id);
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

	void shape_quad_init()
	{
		GLfloat outline_vertices[] = {0.f, 0.f,
									  1.f, 0.f,
									  1.f, 1.f,
									  0.f, 1.f};

		GLfloat fill_vertices[] = {0.f, 0.f,
								   0.f, 1.f,
								   1.f, 0.f,
								   1.f, 1.f};

		quad_vao = vao::generate();
		quad_vbo = vbo::generate();

		vbo::bind(quad_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(fill_vertices), fill_vertices, GL_STATIC_DRAW);

		vao::bind(quad_vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();

		glGenVertexArrays(1, &quad_outline_vao);
		glGenBuffers(1, &quad_outline_vbo);

		vbo::bind(quad_outline_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(outline_vertices), outline_vertices, GL_STATIC_DRAW);

		vao::bind(quad_outline_vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();
	}

	void shape_texture_init()
	{
		// Defines vertices, z is texture coordinates index.
		GLfloat vertices[4][3] = {
				{0.f, 1.f, 0},
				{1.f, 1.f, 1},
				{0.f, 0.f, 2},
				{1.f, 0.f, 3}
		};

		texture_vao = vao::generate();
		texture_vbo = vbo::generate();

		vbo::bind(texture_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

		vao::bind(texture_vao);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();
	}

	GraphicsManager::GraphicsManager() : _graphics_used(GRAPHICS_GL3)
	{}

	void GraphicsManager::init()
	{
		//register_graphics(GRAPHICS_GL1, [](const glm::mat4 &projection) { return (Graphics *) new GraphicsGL1(projection); });
		register_graphics(GRAPHICS_GL3,
						  [this](const Dimension2D_u32 &framebuffer_size)
						  {
							  return (Graphics *) new GraphicsGL3(framebuffer_size, vao, vbo);
						  });
		_graphics_used = GRAPHICS_GL3;

		// Configure VAO/VBO for texture quads
		this->vao = vao::generate();
		this->vbo = vbo::generate();
		vao::bind(this->vao);
		vbo::bind(this->vbo);
		GLfloat text_vertices[6][4] = {
				{0.f, 1.f, 0.0, 1.0},
				{1.f, 0.f, 1.0, 0.0},
				{0.f, 0.f, 0.0, 0.0},

				{0.f, 1.f, 0.0, 1.0},
				{1.f, 1.f, 1.0, 1.0},
				{1.f, 0.f, 1.0, 0.0}
		};
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, text_vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
		vbo::unbind();
		vao::unbind();

		shape_quad_init();
		shape_texture_init();

		if (!shader::compile(IONICENGINE_SHADERS_2DBASIC))
			throw std::runtime_error("Cannot load 2d basic shaders.");
		if (!shader::compile(IONICENGINE_SHADERS_IMAGE))
			throw std::runtime_error("Cannot load image shaders.");
		if (!shader::compile(SHADER_TEXT))
			throw std::runtime_error("Cannot load text shaders.");

		// Configure Framebuffer
		this->framebuffer = fbo::generate();
	}

	void GraphicsManager::register_graphics(const ResourceName &name, const newGraphicsFunction &newGraphics)
	{
		if (_graphics.count(name))
			throw std::invalid_argument(
					"GraphicsManager has already registered the graphics with name '" + name.to_string() + "'!");
		_graphics[name] = newGraphics;
	}

	void GraphicsManager::use_graphics(ResourceName name)
	{
		_graphics_used = std::move(name);
	}

	Graphics *GraphicsManager::new_graphics(const Dimension2D_u32 &framebuffer_size) const
	{
		if (!_graphics.count(_graphics_used))
			return nullptr;
		return _graphics.at(_graphics_used)(framebuffer_size);
	}

	const GraphicsManager *graphics_manager = new GraphicsManager{};

	GraphicsManager *get_graphics_manager()
	{
		return const_cast<GraphicsManager *>(graphics_manager);
	}
}