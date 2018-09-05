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
	namespace graphics
	{
		bool isMouseInBox(int mouseX, int mouseY, int x, int y, uint32_t width, uint32_t height)
		{
			return mouseX >= x && mouseY >= y && mouseX < x + static_cast<int>(width) &&
				   mouseY < y + static_cast<int>(height);
		}
	}

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