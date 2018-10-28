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
		bool is_mouse_in_box(int mouseX, int mouseY, int x, int y, uint32_t width, uint32_t height)
		{
			return mouseX >= x && mouseY >= y && mouseX < x + static_cast<int>(width) &&
				   mouseY < y + static_cast<int>(height);
		}
	}
}