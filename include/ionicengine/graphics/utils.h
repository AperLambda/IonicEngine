/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_UTILS_H
#define IONICENGINE_UTILS_H

#include "../includes.h"

namespace ionicengine
{
	namespace graphics
	{
		bool is_mouse_in_box(int mouseX, int mouseY, int x, int y, uint32_t width, uint32_t height);
	}
}

#endif //IONICENGINE_UTILS_H
