/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_BUFFER_H
#define IONICENGINE_BUFFER_H

#include "../includes.h"

namespace ionicengine
{
	namespace vao
	{
		extern uint32_t IONICENGINE_API generate();

		extern void IONICENGINE_API bind(uint32_t id);

		extern void IONICENGINE_API unbind();
	}

	namespace vbo
	{
		extern uint32_t IONICENGINE_API generate();

		extern void IONICENGINE_API bind(uint32_t id);

		extern void IONICENGINE_API unbind();
	}

	namespace fbo
	{
		extern uint32_t IONICENGINE_API generate();

		extern void IONICENGINE_API bind(uint32_t id);

		extern void IONICENGINE_API unbind();
	}
}

#endif //IONICENGINE_BUFFER_H