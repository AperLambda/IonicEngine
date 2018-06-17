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
	class IONICENGINE_API Monitor
	{
	private:
		GLFWmonitor *_monitor;

	public:
		explicit Monitor(GLFWmonitor *monitor = glfwGetPrimaryMonitor());

		/*!
		 * Gets the name of the monitor.
		 * @return The name of the monitor.
		 */
		std::string getName() const;

		bool isEmpty() const;

		bool operator==(const Monitor &other) const;
	};

	namespace monitor
	{
		extern std::vector<Monitor> IONICENGINE_API getMonitors();

		extern Monitor IONICENGINE_API getPrimaryMonitor();
	}
};

#endif //IONICENGINE_MONITOR_H