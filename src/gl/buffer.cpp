/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/gl/buffer.h"

namespace ionicengine
{
	namespace vao
	{
		// VAO

		// Others

		uint32_t IONICENGINE_API generate()
		{
			uint32_t id;
			glGenVertexArrays(1, &id);
			return id;
		}

		void IONICENGINE_API bind(uint32_t id)
		{
			glBindVertexArray(id);
		}

		void IONICENGINE_API unbind()
		{
			bind(0);
		}
	}

	namespace vbo
	{
		uint32_t IONICENGINE_API generate()
		{
			uint32_t id;
			glGenBuffers(1, &id);
			return id;
		}

		void IONICENGINE_API bind(uint32_t id)
		{
			glBindBuffer(GL_ARRAY_BUFFER, id);
		}

		void IONICENGINE_API unbind()
		{
			bind(0);
		}
	}

	namespace fbo
	{
		uint32_t generate()
		{
			uint32_t id;
			glGenFramebuffers(1, &id);
			return id;
		}

		void IONICENGINE_API bind(uint32_t id)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, id);
		}

		void IONICENGINE_API unbind()
		{
			bind(0);
		}
	}
}