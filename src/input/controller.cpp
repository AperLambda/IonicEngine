/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/input/controller.h"

namespace ionicengine
{
	Controller::Controller(uint8_t id) : joystick(id)
	{}

	uint8_t Controller::get_id() const
	{
		return joystick;
	}

	std::string Controller::get_name() const
	{
		const char *name = glfwGetJoystickName(joystick);
		if (name == nullptr)
			return "NULL";
		return std::string(name);
	}

	std::string Controller::get_guid() const
	{
		const char *name = glfwGetJoystickGUID(joystick);
		if (name == nullptr)
			return "NULL";
		return std::string(name);
	}

	bool Controller::is_connected() const
	{
		return glfwJoystickPresent(joystick) == GLFW_TRUE;
	}

	bool Controller::is_gamepad() const
	{
		return glfwJoystickIsGamepad(joystick) == GLFW_TRUE;
	}

	void Controller::set_axis_inverted(uint8_t axis, bool inverted)
	{
		if (axis < 6)
			axis_inverted[axis] = inverted;
	}

	bool Controller::is_axis_inverted(uint8_t axis) const
	{
		if (axis >= 6)
			return false;
		return axis_inverted[axis];
	}
}