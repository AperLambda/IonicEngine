/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_LIBRARY_H
#define IONICENGINE_LIBRARY_H

#include "ionicengine_exports.h"
#include <string>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#define IONICENGINE_VERSION_MAJOR 0
#define IONICENGINE_VERSION_MINOR 0
#define IONICENGINE_VERSION_PATCH 1

namespace ionicengine
{
	extern std::string IONICENGINE_API getVersion();
}

#endif // IONICENGINE_LIBRARY_H