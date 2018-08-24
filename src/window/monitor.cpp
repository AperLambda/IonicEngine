/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/window/monitor.h"

namespace ionicengine
{
	Monitor::Monitor(GLFWmonitor *monitor) : _monitor(monitor)
	{}

	Monitor::Monitor(const Monitor &monitor) = default;

	Monitor::Monitor(Monitor &&monitor) noexcept : _monitor(monitor._monitor)
	{}

	GLFWmonitor *Monitor::getHandle() const
	{
		return _monitor;
	}

	std::string Monitor::getName() const
	{
		if (_monitor == nullptr)
			return "nullptr";
		return std::string(glfwGetMonitorName(_monitor));
	}

	VideoMode Monitor::getVideoMode() const
	{
		VideoMode vidMode{};
		if (_monitor == nullptr)
			return vidMode;
		auto glfwVidMode = glfwGetVideoMode(_monitor);
		vidMode.width = glfwVidMode->width;
		vidMode.height = glfwVidMode->height;
		vidMode.redBits = glfwVidMode->redBits;
		vidMode.greenBits = glfwVidMode->greenBits;
		vidMode.blueBits = glfwVidMode->blueBits;
		vidMode.refreshRate = glfwVidMode->refreshRate;
		return vidMode;
	}

	bool Monitor::isEmpty() const
	{
		return _monitor == nullptr;
	}

	bool Monitor::operator==(const Monitor &other) const
	{
		return _monitor == other._monitor;
	}

	Monitor &Monitor::operator=(const Monitor &other) = default;

	Monitor &Monitor::operator=(Monitor &&other) noexcept
	{
		if (this != &other)
			_monitor = other._monitor;
		return *this;
	}

	namespace monitor
	{
		std::vector<Monitor> getMonitors()
		{
			int length;
			GLFWmonitor **glfw_monitors = glfwGetMonitors(&length);
			std::vector<Monitor> monitors{static_cast<size_t>(length)};
			for (size_t i = 0; i < length; i++)
			{
				monitors.emplace_back(Monitor{glfw_monitors[i]});
			}
			return monitors;
		}

		const Monitor getPrimaryMonitor()
		{
			return Monitor{glfwGetPrimaryMonitor()};
		}
	}
}