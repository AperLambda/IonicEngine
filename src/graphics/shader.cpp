/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/shader.h"
#include <glm/gtc/type_ptr.hpp>
#include <map>

namespace ionicengine
{
	std::map<std::string, Shader> shaders;

	Shader::Shader() : _id(0)
	{}

	Shader::Shader(uint32_t id) : _id(id)
	{}

	Shader::Shader(const Shader &shader) = default;

	Shader::Shader(Shader &&shader) noexcept : _id(shader._id)
	{}

	uint32_t Shader::get_id() const
	{
		return _id;
	}

	Shader &Shader::use()
	{
		if (_id != 0)
			glUseProgram(_id);
		return *this;
	}

	void Shader::set_float(const std::string &name, float value, bool use_shader)
	{
		if (use_shader)
			this->use();
		glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
	}

	void Shader::set_integer(const std::string &name, int value, bool use_shader)
	{
		if (use_shader)
			this->use();
		glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
	}

	void Shader::set_vector_2f(const std::string &name, float x, float y, bool use_shader)
	{
		if (use_shader)
			this->use();
		glUniform2f(glGetUniformLocation(_id, name.c_str()), x, y);
	}

	void Shader::set_vector_2f(const std::string &name, const glm::vec2 &value, bool use_shader)
	{
		this->set_vector_2f(name, value.x, value.y, use_shader);
	}

	void Shader::setVector3f(const std::string &name, float x, float y, float z, bool use_shader)
	{
		if (use_shader)
			this->use();
		glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
	}

	void Shader::setVector3f(const std::string &name, const glm::vec3 &value, bool use_shader)
	{
		this->setVector3f(name, value.x, value.y, value.z, use_shader);
	}

	void Shader::set_vector_4f(const std::string &name, float x, float y, float z, float w, bool use_shader)
	{
		if (use_shader)
			this->use();
		glUniform4f(glGetUniformLocation(_id, name.c_str()), x, y, z, w);
	}

	void Shader::set_vector_4f(const std::string &name, const glm::vec4 &value, bool use_shader)
	{
		this->set_vector_4f(name, value.x, value.y, value.z, value.w, use_shader);
	}

	void Shader::set_color(const lambdacommon::Color &color, bool use_shader)
	{
		this->set_color("inColor", color, use_shader);

	}

	void Shader::set_color(const std::string &name, const lambdacommon::Color &color, bool use_shader)
	{
		this->set_vector_4f(name, color.red(), color.green(), color.blue(), color.alpha(), use_shader);
	}

	void Shader::set_matrix_4f(const std::string &name, const glm::mat4 &matrix, bool use_shader)
	{
		if (use_shader)
			this->use();
		glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	Shader::operator bool() const
	{
		return _id != 0;
	}

	Shader &Shader::operator=(const Shader &other) = default;

	Shader &Shader::operator=(Shader &&other) noexcept
	{
		if (this != &other)
			_id = other._id;
		return *this;
	}

	bool Shader::operator==(const Shader &shader) const
	{
		return _id == shader._id;
	}

	bool Shader::operator<(const Shader &shader) const
	{
		return _id < shader._id;
	}

	bool check_compile_errors(const lambdacommon::ResourceName &name, uint32_t id, const std::string &type)
	{
		int success;
		char infoLog[1024];
		if (!lambdacommon::lstring::equals_ignore_case(type, "program"))
		{
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(id, 1024, nullptr, infoLog);
				print_error(
						"[IONICENGINE] Error - Shader: Cannot compile shader of type '" + type + "'. | Resource: " +
						name.to_string());
				print_error(infoLog);
				return false;
			}
		}
		else
		{
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(id, 1024, nullptr, infoLog);
				print_error("[IONICENGINE] Error - Shader: Cannot link shader. | Resource: " + name.to_string());
				print_error(infoLog);
				return false;
			}
		}
		return true;
	}

	std::optional<Shader> IONICENGINE_API shader::compile(const lambdacommon::ResourceName &shader_name)
	{
		if (shaders.count(shader_name.to_string()))
			return {shaders.at(shader_name.to_string())};

		auto vertexSource = get_resources_manager().load_resource(shader_name, "vert");
		auto fragmentSource = get_resources_manager().load_resource(shader_name, "frag");

		if (vertexSource.empty() || fragmentSource.empty())
			return {};

		auto vertexCSource = vertexSource.c_str();
		auto fragmentCSource = fragmentSource.c_str();
		// Vertex shader
		auto sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vertexCSource, nullptr);
		glCompileShader(sVertex);
		if (!check_compile_errors(shader_name, sVertex, "vertex"))
			return {};
		// Fragment shader
		auto sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fragmentCSource, nullptr);
		glCompileShader(sFragment);
		if (!check_compile_errors(shader_name, sFragment, "fragment"))
			return {};

		bool hasGeomtryShader = false;
		uint32_t sGeom = 0;
		if (get_resources_manager().does_resource_exist(shader_name, "geom"))
		{
			auto geomSource = get_resources_manager().load_resource(shader_name, "geom").c_str();
			sGeom = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(sGeom, 1, &geomSource, nullptr);
			glCompileShader(sGeom);
			if (!check_compile_errors(shader_name, sGeom, "geometry"))
				return {};
			hasGeomtryShader = true;
		}

		uint32_t id = glCreateProgram();
		glAttachShader(id, sVertex);
		glAttachShader(id, sFragment);
		if (hasGeomtryShader)
			glAttachShader(id, sGeom);
		glLinkProgram(id);
		if (!check_compile_errors(shader_name, id, "program"))
			return {};
		// Delete the shaders
		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
		if (hasGeomtryShader)
			glDeleteShader(sGeom);
		Shader shader{id};
		print_debug("[IonicEngine] Shader '" + shader_name.to_string() + "' loaded successfully with ID '" +
					std::to_string(id) + "'!");
		shaders.insert(std::pair<std::string, Shader>(shader_name.to_string(), shader));
		return {shader};
	}

	bool shader::hasShader(const lambdacommon::ResourceName &shader_name)
	{
		return static_cast<bool>(shaders.count(shader_name.to_string()));
	}

	const Shader &shader::getShader(const lambdacommon::ResourceName &shader_name)
	{
		if (!shaders.count(shader_name.to_string()))
			throw std::runtime_error("Cannot get the shader " + shader_name.to_string());
		return shaders[shader_name.to_string()];
	}
}