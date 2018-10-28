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
#include "../include/ionicengine/graphics/screen.h"

#include <algorithm>
#include <iostream>

namespace ionicengine
{
	lambdacommon::ResourcesManager manager;
	FontManager *font_manager = nullptr;
	std::vector<ScreenManager> *windows = nullptr;
	bool initialized = false;
	bool running = false;
	bool _debug = false;

	bool IONICENGINE_API init(IonicOptions options)
	{
		manager = {options.path};
		_debug = options.debug;
		if (!glfwInit())
			return false;
		font_manager = new FontManager();
		if (!sound::init())
			return false;
		initialized = true;
		InputManager::INPUT_MANAGER.init(options.use_controllers);
		return true;
	}

	void IONICENGINE_API shutdown()
	{
		initialized = false;
		print_debug("[IonicEngine] Shutting down...");
		window::destroy_all();
		sound::shutdown();
		font_manager->shutdown();
		texture::shutdown();
		InputManager::INPUT_MANAGER.shutdown();
		glfwTerminate();
		LCOMMON_DELETE_POINTER(font_manager);
	}

	bool IONICENGINE_API is_running()
	{
		return initialized;
	}

	bool IONICENGINE_API post_init()
	{
		glewExperimental = GL_TRUE;
		return glewInit() == GLEW_OK;
	}

	void IONICENGINE_API run()
	{
		if (windows->empty())
			return;

		running = true;

		while (running)
		{
			if (windows->empty())
				stop();
			else
				for (const ScreenManager &screen_manager : *windows)
				{
					auto window = screen_manager.get_attached_window();
					if (!window || window->should_close())
					{
						if (window)
							window->destroy();

						windows->erase(std::remove(windows->begin(), windows->end(), screen_manager), windows->end());
						continue;
					}

					window->request_context();

					glfwSwapBuffers(window->get_handle());
				}

			glfwPollEvents();
		}
	}

	void IONICENGINE_API stop()
	{
		running = false;
	}

	void IONICENGINE_API print_error(const std::string &message)
	{
		std::cerr << message << std::endl;
	}

	void IONICENGINE_API print_debug(const std::string &message)
	{
		if (_debug)
			std::cout << message << std::endl;
	}

	lambdacommon::ResourcesManager &IONICENGINE_API get_resources_manager()
	{
		return manager;
	}

	FontManager *get_font_manager()
	{
		return font_manager;
	}

	std::string IONICENGINE_API get_version()
	{
		return std::to_string(IONICENGINE_VERSION_MAJOR) + '.' + std::to_string(IONICENGINE_VERSION_MINOR) + '.' +
			   std::to_string(IONICENGINE_VERSION_PATCH);
	}
}