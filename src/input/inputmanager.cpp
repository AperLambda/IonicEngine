/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/input/inputmanager.h"
#include <algorithm>
#include <stdexcept>

namespace ionicengine
{
	void invokeControllerBaseEvent(int joy, int event)
	{
		InputManager &controllers = InputManager::INPUT_MANAGER;
		Controller controller = controllers.getController(static_cast<uint8_t>(joy));
		for (ControllerBaseListener *listener : controllers.getControllerBaseListeners())
		{
			if (event == GLFW_CONNECTED)
				listener->connect(controller);
			else if (event == GLFW_DISCONNECTED)
				listener->disconnect(controller);
		}
	}

	InputManager::InputManager()
	{
		controllers.reserve(GLFW_JOYSTICK_LAST + 1);
		for (uint8_t i = 0; i < GLFW_JOYSTICK_LAST + 1; i++)
			controllers.emplace_back(Controller(i));
	}

	void InputManager::init()
	{
		glfwSetJoystickCallback(invokeControllerBaseEvent);
	}

	Controller InputManager::getController(uint8_t id)
	{
		if (id > controllers.size() && id > GLFW_JOYSTICK_LAST)
			throw std::out_of_range("Cannot get a Controller with the ID: " + std::to_string(id));
		return controllers[id];
	}

	std::vector<Controller> InputManager::getControllers() const
	{
		return controllers;
	}

	void InputManager::addControllerBaseListener(ControllerBaseListener *listener)
	{
		if (!hasControllerBaseListener(listener))
			controllerBaseListeners.push_back(listener);
	}

	bool InputManager::removeControllerBaseListener(ControllerBaseListener *listener)
	{
		if (!hasControllerBaseListener(listener))
			return false;
		controllerBaseListeners
				.erase(std::remove(controllerBaseListeners.begin(), controllerBaseListeners.end(), listener),
				       controllerBaseListeners.end());
		return true;
	}

	bool InputManager::hasControllerBaseListener(ControllerBaseListener *listener)
	{
		return std::find(controllerBaseListeners.begin(), controllerBaseListeners.end(), listener) !=
		       controllerBaseListeners.end();
	}

	std::vector<ControllerBaseListener *> InputManager::getControllerBaseListeners() const
	{
		return controllerBaseListeners;
	}

	InputManager InputManager::INPUT_MANAGER{};
}