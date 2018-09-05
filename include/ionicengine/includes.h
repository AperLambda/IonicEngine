/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_INCLUDES_H
#define IONICENGINE_INCLUDES_H

#include "ionicengine_exports.h"

#define GLFW_INCLUDE_NONE

#define IONIC_DELETE_POINTER(pointer) delete pointer; pointer = nullptr;

#include <GLFW/glfw3.h>
#include <GL/glew.h>

#include <lambdacommon/resources.h>

#endif //IONICENGINE_INCLUDES_H
