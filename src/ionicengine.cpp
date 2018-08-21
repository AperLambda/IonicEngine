/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../include/ionicengine/ionicengine.h"
#include "../include/ionicengine/input/inputmanager.h"
#include "lambdacommon/resources.h"

#include <iostream>

namespace ionicengine
{
	lambdacommon::ResourcesManager manager;
	bool running = false;

	bool IONICENGINE_API init(lambdacommon::fs::FilePath path)
	{
		manager = {path};
		running = true;
		return true;
	}

	void IONICENGINE_API shutdown()
	{
		running = false;
		InputManager::INPUT_MANAGER.shutdown();
	}

	bool IONICENGINE_API isRunning()
	{
		return running;
	}

	void printError(const std::string &message)
	{
		std::cerr << message << std::endl;
	}

	lambdacommon::ResourcesManager &IONICENGINE_API getResourcesManager()
	{
		return manager;
	}

	std::string IONICENGINE_API getVersion()
	{
		return std::to_string(IONICENGINE_VERSION_MAJOR) + '.' + std::to_string(IONICENGINE_VERSION_MINOR) + '.' +
			   std::to_string(IONICENGINE_VERSION_PATCH);
	}
}