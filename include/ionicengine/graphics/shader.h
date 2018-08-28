/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_SHADER_H
#define IONICENGINE_SHADER_H

#include "../ionicengine.h"
#include <lambdacommon/graphics/color.h>
#include <glm/glm.hpp>
#include <optional>

namespace ionicengine
{
	using namespace std::rel_ops;

	class IONICENGINE_API Shader
	{
	private:
		uint32_t _id;

	public:
		Shader();

		Shader(uint32_t id);

		Shader(const Shader &shader);

		Shader(Shader &&shader) noexcept;

		uint32_t getId() const;

		Shader &use();

		void setFloat(const std::string &name, float value, bool useShader = false);

		void setInteger(const std::string &name, int value, bool useShader = false);

		void setVector2f(const std::string &name, float x, float y, bool useShader = false);

		void setVector2f(const std::string &name, const glm::vec2 &value, bool useShader = false);

		void setVector3f(const std::string &name, float x, float y, float z, bool useShader = false);

		void setVector3f(const std::string &name, const glm::vec3 &value, bool useShader = false);

		void setVector4f(const std::string &name, float x, float y, float z, float w, bool useShader = false);

		void setVector4f(const std::string &name, const glm::vec4 &value, bool useShader = false);

		void setColor(const lambdacommon::Color &color, bool useShader = false);

		void setColor(const std::string &name, const lambdacommon::Color &color, bool useShader = false);

		void setMatrix4f(const std::string &name, const glm::mat4 &matrix, bool useShader = false);

		explicit operator bool() const;

		Shader &operator=(const Shader &other);

		Shader &operator=(Shader &&other) noexcept;

		bool operator==(const Shader &shader) const;

		bool operator<(const Shader &shader) const;
	};

	namespace shader
	{
		extern std::optional<Shader> IONICENGINE_API compile(const lambdacommon::ResourceName &shaderName);

		extern bool IONICENGINE_API hasShader(const lambdacommon::ResourceName &shaderName);

		extern const Shader &IONICENGINE_API getShader(const lambdacommon::ResourceName &shaderName);
	}
}

#endif //IONICENGINE_SHADER_H
