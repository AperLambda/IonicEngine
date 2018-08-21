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
	std::map<lambdacommon::ResourceName, Shader> shaders;

	Shader::Shader(uint32_t id) : _id(id)
	{}

	uint32_t Shader::getId() const
	{
		return _id;
	}

	Shader &Shader::use()
	{
		if (_id != 0)
			glUseProgram(_id);
		return *this;
	}

	void Shader::setFloat(const std::string &name, float value, bool useShader)
	{
		if (useShader)
			this->use();
		glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
	}

	void Shader::setInteger(const std::string &name, int value, bool useShader)
	{
		if (useShader)
			this->use();
		glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
	}

	void Shader::setVector2f(const std::string &name, float x, float y, bool useShader)
	{
		if (useShader)
			this->use();
		glUniform2f(glGetUniformLocation(_id, name.c_str()), x, y);
	}

	void Shader::setVector2f(const std::string &name, const glm::vec2 &value, bool useShader)
	{
		this->setVector2f(name, value.x, value.y, useShader);
	}

	void Shader::setVector3f(const std::string &name, float x, float y, float z, bool useShader)
	{
		if (useShader)
			this->use();
		glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
	}

	void Shader::setVector3f(const std::string &name, const glm::vec3 &value, bool useShader)
	{
		this->setVector3f(name, value.x, value.y, value.z, useShader);
	}

	void Shader::setVector4f(const std::string &name, float x, float y, float z, float w, bool useShader)
	{
		if (useShader)
			this->use();
		glUniform4f(glGetUniformLocation(_id, name.c_str()), x, y, z, w);
	}

	void Shader::setVector4f(const std::string &name, const glm::vec4 &value, bool useShader)
	{
		this->setVector4f(name, value.x, value.y, value.z, value.w, useShader);
	}

	void Shader::setMatrix4f(const std::string &name, const glm::mat4 &matrix, bool useShader)
	{
		if (useShader)
			this->use();
		glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
	}

	Shader::operator bool() const
	{
		return _id != 0;
	}

	bool checkCompileErrors(const lambdacommon::ResourceName &name, uint32_t id, const std::string &type)
	{
		int success;
		char infoLog[1024];
		if (!lambdacommon::lambdastring::equalsIgnoreCase(type, "program"))
		{
			glGetShaderiv(id, GL_COMPILE_STATUS, &success);
			if (!success)
			{
				glGetShaderInfoLog(id, 1024, nullptr, infoLog);
				printError(
						"[IONICENGINE] Error - Shader: Cannot compile shader of type \"" + type + "\". | Resource: " +
						name.getDomain() + ":" + name.getName());
				return false;
			}
		}
		else
		{
			glGetProgramiv(id, GL_LINK_STATUS, &success);
			if (!success)
			{
				glGetProgramInfoLog(id, 1024, nullptr, infoLog);
				printError("[IONICENGINE] Error - Shader: Cannot link shader. | Resource: " + name.getDomain() + ":" +
						   name.getName());
				return false;
			}
		}
		return true;
	}

	std::optional<Shader> IONICENGINE_API shader::compile(const lambdacommon::ResourceName &resourceName)
	{
		if (shaders.count(resourceName))
			return {shaders.at(resourceName)};

		auto vertexSource = getResourcesManager().loadResource(resourceName, "vert");
		auto fragmentSource = getResourcesManager().loadResource(resourceName, "frag");

		if (vertexSource.empty() || fragmentSource.empty())
			return {};

		auto vertexCSource = vertexSource.c_str();
		auto fragmentCSource = fragmentSource.c_str();
		// Vertex shader
		auto sVertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(sVertex, 1, &vertexCSource, nullptr);
		glCompileShader(sVertex);
		if (!checkCompileErrors(resourceName, sVertex, "vertex"))
			return {};
		// Fragment shader
		auto sFragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(sFragment, 1, &fragmentCSource, nullptr);
		glCompileShader(sFragment);
		if (!checkCompileErrors(resourceName, sFragment, "fragment"))
			return {};

		bool hasGeomtryShader = false;
		uint32_t sGeom = 0;
		if (getResourcesManager().doesResourceExist(resourceName, "geom"))
		{
			auto geomSource = getResourcesManager().loadResource(resourceName, "geom").c_str();
			sGeom = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(sGeom, 1, &geomSource, nullptr);
			glCompileShader(sGeom);
			if (!checkCompileErrors(resourceName, sGeom, "geometry"))
				return {};
			hasGeomtryShader = true;
		}

		uint32_t id = glCreateProgram();
		glAttachShader(id, sVertex);
		glAttachShader(id, sFragment);
		if (hasGeomtryShader)
			glAttachShader(id, sGeom);
		glLinkProgram(id);
		if (!checkCompileErrors(resourceName, id, "program"))
			return {};
		// Delete the shaders
		glDeleteShader(sVertex);
		glDeleteShader(sFragment);
		if (hasGeomtryShader)
			glDeleteShader(sGeom);
		Shader shader{id};
		shaders[resourceName] = shader;
		return {shader};
	}
}