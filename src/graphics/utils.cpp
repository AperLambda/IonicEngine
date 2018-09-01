/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/graphics/utils.h"

namespace ionicengine
{
	namespace vao
	{
		void bind(uint32_t id)
		{
			glBindVertexArray(id);
		}

		void unbind()
		{
			bind(0);
		}
	}

	namespace vbo
	{
		void bind(uint32_t id)
		{
			glBindBuffer(GL_ARRAY_BUFFER, id);
		}

		void unbind()
		{
			bind(0);
		}
	}
}