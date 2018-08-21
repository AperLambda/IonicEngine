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

	uint8_t Controller::getId() const
	{
		return joystick;
	}

	std::string Controller::getName() const
	{
		const char *name = glfwGetJoystickName(joystick);
		if (name == nullptr)
			return "NULL";
		return std::string(name);
	}

	std::string Controller::getGUID() const
	{
		const char *name = glfwGetJoystickGUID(joystick);
		if (name == nullptr)
			return "NULL";
		return std::string(name);
	}

	bool Controller::isConnected() const
	{
		return glfwJoystickPresent(joystick) == GLFW_TRUE;
	}

	bool Controller::isGamepad() const
	{
		return glfwJoystickIsGamepad(joystick) == GLFW_TRUE;
	}

	void Controller::setAxisInverted(uint8_t axis, bool inverted)
	{
		if (axis < 6)
			axisInverted[axis] = inverted;
	}

	bool Controller::isAxisInverted(uint8_t axis) const
	{
		if (axis >= 6)
			return false;
		return axisInverted[axis];
	}
}