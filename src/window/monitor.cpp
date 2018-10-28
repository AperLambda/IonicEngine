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

	GLFWmonitor *Monitor::get_handle() const
	{
		return _monitor;
	}

	std::string Monitor::get_name() const
	{
		if (_monitor == nullptr)
			return "nullptr";
		return std::string(glfwGetMonitorName(_monitor));
	}

	VideoMode Monitor::get_video_mode() const
	{
		VideoMode vid_mode{};
		if (_monitor == nullptr)
			return vid_mode;
		auto glfwVidMode = glfwGetVideoMode(_monitor);
		vid_mode.width = glfwVidMode->width;
		vid_mode.height = glfwVidMode->height;
		vid_mode.red_bits = glfwVidMode->redBits;
		vid_mode.green_bits = glfwVidMode->greenBits;
		vid_mode.blue_bits = glfwVidMode->blueBits;
		vid_mode.refresh_rate = glfwVidMode->refreshRate;
		return vid_mode;
	}

	bool Monitor::is_empty() const
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
		std::vector<Monitor> get_monitors()
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

		const Monitor get_primary_monitor()
		{
			return Monitor{glfwGetPrimaryMonitor()};
		}
	}
}