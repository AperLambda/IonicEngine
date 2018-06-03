/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_SCREEN_H
#define IONICENGINE_SCREEN_H

#include "graphics.h"

namespace ionicengine
{
	class Screen
	{
	public:
		virtual void draw(Graphics *graphics) = 0;
	};
}

#endif //IONICENGINE_SCREEN_H
