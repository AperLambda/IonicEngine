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

		uint32_t get_id() const;

		Shader &use();

		void set_float(const std::string &name, float value, bool use_shader = false);

		void set_integer(const std::string &name, int value, bool use_shader = false);

		void set_vector_2f(const std::string &name, float x, float y, bool use_shader = false);

		void set_vector_2f(const std::string &name, const glm::vec2 &value, bool use_shader = false);

		void setVector3f(const std::string &name, float x, float y, float z, bool use_shader = false);

		void setVector3f(const std::string &name, const glm::vec3 &value, bool use_shader = false);

		void set_vector_4f(const std::string &name, float x, float y, float z, float w, bool use_shader = false);

		void set_vector_4f(const std::string &name, const glm::vec4 &value, bool use_shader = false);

		void set_color(const lambdacommon::Color &color, bool use_shader = false);

		void set_color(const std::string &name, const lambdacommon::Color &color, bool use_shader = false);

		void set_matrix_4f(const std::string &name, const glm::mat4 &matrix, bool use_shader = false);

		explicit operator bool() const;

		Shader &operator=(const Shader &other);

		Shader &operator=(Shader &&other) noexcept;

		bool operator==(const Shader &shader) const;

		bool operator<(const Shader &shader) const;
	};

	namespace shader
	{
		extern std::optional<Shader> IONICENGINE_API compile(const lambdacommon::ResourceName &shader_name);

		extern bool IONICENGINE_API hasShader(const lambdacommon::ResourceName &shader_name);

		extern const Shader &IONICENGINE_API getShader(const lambdacommon::ResourceName &shader_name);
	}
}

#endif //IONICENGINE_SHADER_H
