/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_MONITOR_H
#define IONICENGINE_MONITOR_H

#include "../ionicengine.h"
#include <vector>

namespace ionicengine
{
	class Monitor
	{
	private:
		GLFWmonitor *_monitor;

	public:
		Monitor(GLFWmonitor *monitor = glfwGetPrimaryMonitor());

		std::string getName();
	};

	namespace monitor
	{
		extern std::vector<Monitor> IONICENGINE_API getMonitors();

		extern Monitor IONICENGINE_API getPrimaryMonitor();
	}
};

#endif //IONICENGINE_MONITOR_H