/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/window/monitor.h"

using namespace std;

namespace ionicengine
{
	Monitor::Monitor(GLFWmonitor *monitor) : _monitor(monitor)
	{}

	string Monitor::getName()
	{
		const char *name = glfwGetMonitorName(_monitor);
		if (name == nullptr)
			return "NULL";
		return string(name);
	}

	namespace monitor
	{
		Monitor primaryMonitor{};

		vector<Monitor> getMonitors()
		{
			int length;
			GLFWmonitor **glfw_monitors = glfwGetMonitors(&length);
			vector<Monitor> monitors{static_cast<size_t>(length)};
			for (size_t i = 0; i < length; i++)
			{
				monitors.emplace_back({glfw_monitors[i]});
			}
			return monitors;
		}

		Monitor getPrimaryMonitor()
		{
			return primaryMonitor;
		}
	}
}