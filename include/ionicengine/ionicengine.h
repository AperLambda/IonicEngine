/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_LIBRARY_H
#define IONICENGINE_LIBRARY_H

#include "ionicengine_exports.h"
#include <lambdacommon/resources.h>

#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>
#include <GL/glew.h>

#define IONICENGINE_VERSION_MAJOR 0
#define IONICENGINE_VERSION_MINOR 0
#define IONICENGINE_VERSION_PATCH 1

namespace ionicengine
{
	extern bool IONICENGINE_API init(lambdacommon::fs::FilePath path = lambdacommon::fs::getCurrentWorkingDirectory());

	/*!
	 * Shutdowns the library.
	 */
	extern void IONICENGINE_API shutdown();

	extern bool IONICENGINE_API isRunning();

	extern void IONICENGINE_API printError(const std::string &message);

	extern lambdacommon::ResourcesManager &IONICENGINE_API getResourcesManager();

	extern std::string IONICENGINE_API getVersion();
}

#endif // IONICENGINE_LIBRARY_H