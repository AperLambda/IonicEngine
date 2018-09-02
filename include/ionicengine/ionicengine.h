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
#include "graphics/font.h"
#include "sound/sound.h"

#define IONICENGINE_VERSION_MAJOR 0
#define IONICENGINE_VERSION_MINOR 0
#define IONICENGINE_VERSION_PATCH 1

#define IONICENGINE_NULL_RESOURCE lambdacommon::ResourceName("ionicengine", "null")
#define IONICENGINE_GRAPHICS_GL3 lambdacommon::ResourceName("ionicengine", "graphics/gl3")
#define IONICENGINE_OVERLAYS_FPS lambdacommon::ResourceName("ionicengine", "overlays/fps")
#define IONICENGINE_SHADERS_2DBASIC lambdacommon::ResourceName("ionicengine", "shaders/2dbasic")
#define IONICENGINE_SHADERS_IMAGE lambdacommon::ResourceName("ionicengine", "shaders/image")
#define IONICENGINE_SHADERS_TEXT lambdacommon::ResourceName("ionicengine", "shaders/text")

namespace ionicengine
{
	const lambdacommon::ResourceName GRAPHICS_GL3 = IONICENGINE_GRAPHICS_GL3;

	const lambdacommon::ResourceName SHADER_TEXT = IONICENGINE_SHADERS_TEXT;

	struct IonicOptions
	{
		bool useControllers = true;
		bool debug = false;
		lambdacommon::fs::FilePath path = lambdacommon::fs::getCurrentWorkingDirectory();
	};

	extern bool IONICENGINE_API init(IonicOptions options);

	/*!
	 * Shutdowns the library.
	 */
	extern void IONICENGINE_API shutdown();

	extern bool IONICENGINE_API isRunning();

	extern void IONICENGINE_API printError(const std::string &message);

	extern void IONICENGINE_API printDebug(const std::string &message);

	extern lambdacommon::ResourcesManager &IONICENGINE_API getResourcesManager();

	extern FontManager IONICENGINE_API getFontManager();

	extern std::string IONICENGINE_API getVersion();
}

#endif // IONICENGINE_LIBRARY_H