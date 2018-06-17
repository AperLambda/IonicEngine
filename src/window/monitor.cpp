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

	string Monitor::getName() const
	{
		if (_monitor == nullptr)
			return to_string((long) _monitor);
		return string(glfwGetMonitorName(_monitor));
	}

	bool Monitor::isEmpty() const
	{
		return _monitor == nullptr;
	}

	bool Monitor::operator==(const Monitor &other) const
	{
		return _monitor == other._monitor;
	}

	namespace monitor
	{
		Monitor primaryMonitor{glfwGetPrimaryMonitor()};

		vector<Monitor> getMonitors()
		{
			int length;
			GLFWmonitor **glfw_monitors = glfwGetMonitors(&length);
			vector<Monitor> monitors{static_cast<size_t>(length)};
			for (size_t i = 0; i < length; i++)
			{
				monitors.emplace_back(Monitor{glfw_monitors[i]});
			}
			return monitors;
		}

		Monitor getPrimaryMonitor()
		{
			if (primaryMonitor.isEmpty())
				primaryMonitor = Monitor{};
			return primaryMonitor;
		}
	}
}