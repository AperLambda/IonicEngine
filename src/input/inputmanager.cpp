/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include "../../include/ionicengine/input/inputmanager.h"
#include "../../include/ionicengine/graphics/screen.h"
#include <algorithm>
#include <stdexcept>
#include <thread>

namespace ionicengine
{
	void invokeControllerBaseEvent(int joy, int event)
	{
		InputManager &controllers = InputManager::INPUT_MANAGER;
		auto controller = controllers.getController(static_cast<uint8_t>(joy));
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
			controllers.emplace_back(new Controller{i});
	}

	InputManager::~InputManager()
	{
		delete inputThread;
		for (auto controller : controllers)
			delete controller;
	}

	uint32_t buttonStates[GLFW_JOYSTICK_LAST + 1][GLFW_GAMEPAD_BUTTON_LAST + 1];
	float axesStates[GLFW_JOYSTICK_LAST + 1][GLFW_GAMEPAD_AXIS_LAST + 1];
	bool triggersAsButton[GLFW_JOYSTICK_LAST + 1];

	void checkButton(const Controller &controller, uint8_t button, GLFWgamepadstate state)
	{
		if (state.buttons[button])
		{
			if (buttonStates[controller.getId()][button] != 0)
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.getControllerInputListeners())
					listener->onButtonRepeat(controller, button);
			else if (buttonStates[controller.getId()][button] == 0)
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.getControllerInputListeners())
					listener->onButtonPress(controller, button);

			buttonStates[controller.getId()][button] = buttonStates[controller.getId()][button] + 1;
		}
		else
		{
			if (buttonStates[controller.getId()][button] > 0)
			{
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.getControllerInputListeners())
					listener->onButtonRelease(controller, button);
				buttonStates[controller.getId()][button] = 0;
			}
		}
	}

	void checkAxis(const Controller &controller, uint8_t axis, GLFWgamepadstate state, float releaseNumber)
	{
		float axisValue = state.axes[axis];
		if (controller.isAxisInverted(axis))
			axisValue = axisValue * -1;
		if (axisValue == releaseNumber)
		{
			if (axesStates[controller.getId()][axis] != releaseNumber)
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.getControllerInputListeners())
					listener->onAxisRelease(controller, axis);
		}
		else if (releaseNumber == 0.0f)
		{
			if (axisValue > -0.15f && axisValue < 0.15f)
			{
				if (!(axesStates[controller.getId()][axis] > -0.15f && axesStates[controller.getId()][axis] < 0.15f))
					for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.getControllerInputListeners())
						listener->onAxisRelease(controller, axis);
			}
			else
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.getControllerInputListeners())
					listener->onAxisMove(controller, axis, axisValue);
		}
		else
		{
			for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.getControllerInputListeners())
				listener->onAxisMove(controller, axis, axisValue);
		}
		axesStates[controller.getId()][axis] = axisValue;
	}

	void updateControllers()
	{
		clock_t startTime = clock() / (CLOCKS_PER_SEC / 1000);
		for (bool &j : triggersAsButton)
			j = true;
		while (ionicengine::isRunning())
		{
			clock_t currentTime = (clock() / (CLOCKS_PER_SEC / 1000)) - startTime;
			if (currentTime >= 50)
			{
				// Update
				if (!InputManager::INPUT_MANAGER.getControllerInputListeners().empty())
					for (Controller *controller : InputManager::INPUT_MANAGER.getControllers())
					{
						if (controller->isConnected() && controller->isGamepad())
						{
							GLFWgamepadstate state;

							if (glfwGetGamepadState(controller->getId(), &state))
							{
								for (uint8_t button = 0; button < GLFW_GAMEPAD_BUTTON_LAST + 1; button++)
									checkButton(*controller, button, state);
								for (uint8_t axis = 0; axis < GLFW_GAMEPAD_AXIS_RIGHT_Y + 1; axis++)
									checkAxis(*controller, axis, state, 0.0f);
								if (triggersAsButton[controller->getId()])
								{
									if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] < 0 ||
										state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] < 0)
										triggersAsButton[controller->getId()] = false;
									checkAxis(*controller, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, state, 0.0f);
									checkAxis(*controller, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, state, 0.0f);
								}
								else
								{
									checkAxis(*controller, GLFW_GAMEPAD_AXIS_LEFT_TRIGGER, state, -1.0f);
									checkAxis(*controller, GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER, state, -1.0f);
								}
							}
						}
					}

				// Reset timer.
				startTime = (clock() / (CLOCKS_PER_SEC / 1000));
			}
		}
	}

	void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		auto ionicWindow = window::getByHandle(window);
		if (!ionicWindow)
			ionicWindow = {Window{window}};
		for (KeyboardListener *listener : InputManager::INPUT_MANAGER.getKeyboardListeners())
		{
			listener->onKeyInput(ionicWindow.value(), key, scancode, static_cast<InputAction>(action), mods);
		}
	}

	void char_callback(GLFWwindow *window, unsigned int codepoint)
	{
		auto ionicWindow = window::getByHandle(window);
		if (!ionicWindow)
			ionicWindow = {Window{window}};
		for (KeyboardListener *listener : InputManager::INPUT_MANAGER.getKeyboardListeners())
		{
			listener->onCharInput(ionicWindow.value(), codepoint);
		}
	}

	void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
	{
		auto ionicWindow = window::getByHandle(window);
		if (!ionicWindow)
			ionicWindow = {Window{window}};
		if (!screen::getScreenManager(ionicWindow.value())->onMouseButton(button, static_cast<InputAction>(action),
																		  mods))
			for (MouseListener *listener : InputManager::INPUT_MANAGER.getMouseListeners())
			{
				listener->onMouseButton(ionicWindow.value(), button, static_cast<InputAction>(action), mods);
			}
	}

	void mouse_position_callback(GLFWwindow *window, double x, double y)
	{
		auto ionicWindow = window::getByHandle(window);
		if (!ionicWindow)
			ionicWindow = {Window{window}};
		for (MouseListener *listener : InputManager::INPUT_MANAGER.getMouseListeners())
		{
			listener->onMousePosition(ionicWindow.value(), x, y);
		}
	}

	void mouse_enter_callback(GLFWwindow *window, int entered)
	{
		auto ionicWindow = window::getByHandle(window);
		if (!ionicWindow)
			ionicWindow = {Window{window}};
		for (MouseListener *listener : InputManager::INPUT_MANAGER.getMouseListeners())
		{
			if (entered)
				listener->onMouseEnter(ionicWindow.value());
			else
				listener->onMouseExit(ionicWindow.value());
		}
	}

	void scroll(GLFWwindow *window, double xoffset, double yoffset)
	{
		auto ionicWindow = window::getByHandle(window);
		if (!ionicWindow)
			ionicWindow = {Window{window}};
		for (MouseListener *listener : InputManager::INPUT_MANAGER.getMouseListeners())
		{
			listener->onMouseScroll(ionicWindow.value(), xoffset, yoffset);
		}
	}

	void InputManager::init(bool useController)
	{
		glfwSetJoystickCallback(invokeControllerBaseEvent);

		if (useController)
			inputThread = new std::thread{updateControllers};
	}

	void InputManager::shutdown()
	{
		if (inputThread != nullptr)
			inputThread->join();
	}

	void InputManager::attachWindow(const Window &window)
	{
		glfwSetKeyCallback(window.getHandle(), key_callback);
		glfwSetCharCallback(window.getHandle(), char_callback);
		glfwSetMouseButtonCallback(window.getHandle(), mouse_button_callback);
		glfwSetCursorPosCallback(window.getHandle(), mouse_position_callback);
		glfwSetCursorEnterCallback(window.getHandle(), mouse_enter_callback);
		glfwSetScrollCallback(window.getHandle(), scroll);
	}

	std::string InputManager::getKeyName(int key, int scancode)
	{
		auto keyName = glfwGetKeyName(key, scancode);
		if (keyName == nullptr)
			return "NULL";
		return {keyName};
	}

	void InputManager::addKeyboardListener(KeyboardListener *listener)
	{
		if (!hasKeyboardListener(listener))
			keyboardListeners.push_back(listener);
	}

	bool InputManager::removeKeyboardListener(KeyboardListener *listener)
	{
		if (!hasKeyboardListener(listener))
			return false;
		keyboardListeners.erase(std::remove(keyboardListeners.begin(), keyboardListeners.end(), listener),
								keyboardListeners.end());
		return true;
	}

	bool InputManager::hasKeyboardListener(KeyboardListener *listener)
	{
		return std::find(keyboardListeners.begin(), keyboardListeners.end(), listener) !=
			   keyboardListeners.end();
	}

	std::vector<KeyboardListener *> InputManager::getKeyboardListeners() const
	{
		return keyboardListeners;
	}

	void InputManager::addMouseListener(MouseListener *listener)
	{
		if (!hasMouseListener(listener))
			mouseListeners.push_back(listener);
	}

	bool InputManager::removeMouseListener(MouseListener *listener)
	{
		if (!hasMouseListener(listener))
			return false;
		mouseListeners.erase(std::remove(mouseListeners.begin(), mouseListeners.end(), listener),
							 mouseListeners.end());
		return true;
	}

	bool InputManager::hasMouseListener(MouseListener *listener)
	{
		return std::find(mouseListeners.begin(), mouseListeners.end(), listener) !=
			   mouseListeners.end();
	}

	std::vector<MouseListener *> InputManager::getMouseListeners() const
	{
		return mouseListeners;
	}

	Controller *InputManager::getController(uint8_t id)
	{
		if (id > controllers.size() && id > GLFW_JOYSTICK_LAST)
			throw std::out_of_range("Cannot get a Controller with the ID: " + std::to_string(id));
		return controllers[id];
	}

	std::vector<Controller *> InputManager::getControllers() const
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

	void InputManager::addControllerInputListener(ControllerInputListener *listener)
	{
		if (!hasControllerInputListener(listener))
			controllerInputListeners.push_back(listener);
	}

	bool InputManager::removeControllerInputListener(ControllerInputListener *listener)
	{
		if (!hasControllerInputListener(listener))
			return false;
		controllerInputListeners
				.erase(std::remove(controllerInputListeners.begin(), controllerInputListeners.end(), listener),
					   controllerInputListeners.end());
		return true;
	}

	bool InputManager::hasControllerInputListener(ControllerInputListener *listener)
	{
		return std::find(controllerInputListeners.begin(), controllerInputListeners.end(), listener) !=
			   controllerInputListeners.end();
	}

	std::vector<ControllerInputListener *> InputManager::getControllerInputListeners() const
	{
		return controllerInputListeners;
	}

	InputManager InputManager::INPUT_MANAGER{};
}