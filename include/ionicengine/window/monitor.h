/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
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
	using namespace std::rel_ops;

	struct VideoMode
	{
		/*!
		 * The width, in screen coordinates, of the video mode.
     	 */
		int width;
		/*!
		 * The height, in screen coordinates, of the video mode.
		 */
		int height;
		/*!
		 * The bit depth of the red channel of the video mode.
		 */
		int red_bits;
		/*!
		 * The bit depth of the green channel of the video mode.
		 */
		int green_bits;
		/*!
		 * The bit depth of the blue channel of the video mode.
		 */
		int blue_bits;
		/*!
		 * The refresh rate, in Hz, of the video mode.
		 */
		int refresh_rate;
	};

	class IONICENGINE_API Monitor
	{
	private:
		GLFWmonitor *_monitor;

	public:
		explicit Monitor(GLFWmonitor *monitor = glfwGetPrimaryMonitor());

		Monitor(const Monitor &monitor);

		Monitor(Monitor &&monitor) noexcept;

		/*!
		 * Gets the GLFW handle of the monitor.
		 * @return The handle of the monitor.
		 */
		GLFWmonitor *get_handle() const;

		/*!
		 * Gets the name of the monitor.
		 * @return The name of the monitor.
		 */
		std::string get_name() const;

		/*!
		 * Returns the current mode of this monitor.
		 * @return The current mode of the monitor.
		 */
		VideoMode get_video_mode() const;

		bool is_empty() const;

		bool operator==(const Monitor &other) const;

		Monitor &operator=(const Monitor &other);

		Monitor &operator=(Monitor &&other) noexcept;
	};

	namespace monitor
	{
		extern std::vector<Monitor> IONICENGINE_API get_monitors();

		extern const Monitor IONICENGINE_API get_primary_monitor();
	}
};

#endif //IONICENGINE_MONITOR_H