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
	namespace vao
	{
		void bind(uint32_t id);

		void unbind();
	}

	namespace vbo
	{
		void bind(uint32_t id);

		void unbind();
	}
}

#endif //IONICENGINE_UTILS_H
