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
#include "../include/ionicengine/window/window.h"
#include "../include/ionicengine/graphics/graphics.h"

#include <iostream>

namespace ionicengine
{
	lambdacommon::ResourcesManager manager;
	FontManager *fontManager = nullptr;
	bool running = false;
	bool _debug = false;

	bool IONICENGINE_API init(bool debug, lambdacommon::fs::FilePath path)
	{
		manager = {path};
		_debug = debug;
		fontManager = new FontManager();
		running = true;
		return true;
	}

	void IONICENGINE_API shutdown()
	{
		running = false;
		window::destroyAll();
		InputManager::INPUT_MANAGER.shutdown();
	}

	bool IONICENGINE_API isRunning()
	{
		return running;
	}

	void IONICENGINE_API printError(const std::string &message)
	{
		std::cerr << message << std::endl;
	}

	void IONICENGINE_API printDebug(const std::string &message)
	{
		if (_debug)
			std::cout << message << std::endl;
	}

	lambdacommon::ResourcesManager &IONICENGINE_API getResourcesManager()
	{
		return manager;
	}

	FontManager getFontManager()
	{
		return *fontManager;
	}

	std::string IONICENGINE_API getVersion()
	{
		return std::to_string(IONICENGINE_VERSION_MAJOR) + '.' + std::to_string(IONICENGINE_VERSION_MINOR) + '.' +
			   std::to_string(IONICENGINE_VERSION_PATCH);
	}
}