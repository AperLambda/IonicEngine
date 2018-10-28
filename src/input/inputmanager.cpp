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
#include <lambdacommon/system/time.h>
#include <algorithm>
#include <stdexcept>
#include <thread>

namespace ionicengine
{
	void invoke_controller_base_event(int joy, int event)
	{
		InputManager &controllers = InputManager::INPUT_MANAGER;
		auto controller = controllers.get_controller(static_cast<uint8_t>(joy));
		for (ControllerBaseListener *listener : controllers.get_controller_base_listeners())
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
		delete input_thread;
		for (auto controller : controllers)
			delete controller;
	}

	uint32_t button_states[GLFW_JOYSTICK_LAST + 1][GLFW_GAMEPAD_BUTTON_LAST + 1];
	float axes_states[GLFW_JOYSTICK_LAST + 1][GLFW_GAMEPAD_AXIS_LAST + 1];
	bool triggers_as_button[GLFW_JOYSTICK_LAST + 1];

	void check_button(const Controller &controller, uint8_t button, GLFWgamepadstate state)
	{
		if (state.buttons[button])
		{
			if (button_states[controller.get_id()][button] != 0)
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.get_controller_input_listeners())
					listener->on_button_repeat(controller, button);
			else if (button_states[controller.get_id()][button] == 0)
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.get_controller_input_listeners())
					listener->on_button_press(controller, button);

			button_states[controller.get_id()][button] = button_states[controller.get_id()][button] + 1;
		}
		else
		{
			if (button_states[controller.get_id()][button] > 0)
			{
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.get_controller_input_listeners())
					listener->on_button_release(controller, button);
				button_states[controller.get_id()][button] = 0;
			}
		}
	}

	void checkAxis(const Controller &controller, uint8_t axis, GLFWgamepadstate state, float release_number)
	{
		float axis_value = state.axes[axis];
		if (controller.is_axis_inverted(axis))
			axis_value = -axis_value;
		if (axis_value == release_number)
		{
			if (axes_states[controller.get_id()][axis] != release_number)
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.get_controller_input_listeners())
					listener->on_axis_release(controller, axis);
		}
		else if (release_number == 0.0f)
		{
			if (axis_value > -0.15f && axis_value < 0.15f)
			{
				if (!(axes_states[controller.get_id()][axis] > -0.15f &&
					  axes_states[controller.get_id()][axis] < 0.15f))
					for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.get_controller_input_listeners())
						listener->on_axis_release(controller, axis);
			}
			else
				for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.get_controller_input_listeners())
					listener->on_axis_move(controller, axis, axis_value);
		}
		else
		{
			for (ControllerInputListener *listener : InputManager::INPUT_MANAGER.get_controller_input_listeners())
				listener->on_axis_move(controller, axis, axis_value);
		}
		axes_states[controller.get_id()][axis] = axis_value;
	}

	void update_controllers()
	{
		time_t start_time = lambdacommon::time::get_time_millis();
		for (bool &j : triggers_as_button)
			j = true;
		while (ionicengine::is_running())
		{
			time_t current_time = lambdacommon::time::get_time_millis() - start_time;
			if (current_time >= 50)
			{
				// Update
				if (!InputManager::INPUT_MANAGER.get_controller_input_listeners().empty())
					for (Controller *controller : InputManager::INPUT_MANAGER.get_controllers())
					{
						if (controller->is_connected() && controller->is_gamepad())
						{
							GLFWgamepadstate state;

							if (glfwGetGamepadState(controller->get_id(), &state))
							{
								for (uint8_t button = 0; button < GLFW_GAMEPAD_BUTTON_LAST + 1; button++)
									check_button(*controller, button, state);
								for (uint8_t axis = 0; axis < GLFW_GAMEPAD_AXIS_RIGHT_Y + 1; axis++)
									checkAxis(*controller, axis, state, 0.0f);
								if (triggers_as_button[controller->get_id()])
								{
									if (state.axes[GLFW_GAMEPAD_AXIS_LEFT_TRIGGER] < 0 ||
										state.axes[GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER] < 0)
										triggers_as_button[controller->get_id()] = false;
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
				start_time = lambdacommon::time::get_time_millis();
			}
		}
	}

	void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		auto ionic_window = window::get_by_handle(window);
		if (!ionic_window)
			ionic_window = {Window{window}};
		if (!screen::get_screen_manager(ionic_window.value())->on_key_input(key, scancode,
																			static_cast<InputAction>(action), mods))
			for (KeyboardListener *listener : InputManager::INPUT_MANAGER.get_keyboard_listeners())
			{
				listener->on_key_input(ionic_window.value(), key, scancode, static_cast<InputAction>(action), mods);
			}
	}

	void char_callback(GLFWwindow *window, unsigned int codepoint)
	{
		auto ionic_window = window::get_by_handle(window);
		if (!ionic_window)
			ionic_window = {Window{window}};
		for (KeyboardListener *listener : InputManager::INPUT_MANAGER.get_keyboard_listeners())
		{
			listener->on_char_input(ionic_window.value(), codepoint);
		}
	}

	void mouse_button_callback(GLFWwindow *window, int button, int action, int mods)
	{
		auto ionic_window = window::get_by_handle(window);
		if (!ionic_window)
			ionic_window = {Window{window}};
		if (!screen::get_screen_manager(ionic_window.value())->on_mouse_button(button, static_cast<InputAction>(action),
																			   mods))
			for (MouseListener *listener : InputManager::INPUT_MANAGER.get_mouse_listeners())
			{
				listener->on_mouse_button(ionic_window.value(), button, static_cast<InputAction>(action), mods);
			}
	}

	void mouse_position_callback(GLFWwindow *window, double x, double y)
	{
		auto ionic_window = window::get_by_handle(window);
		if (!ionic_window)
			ionic_window = {Window{window}};
		if (!screen::get_screen_manager(ionic_window.value())->on_mouse_move(static_cast<int>(x), static_cast<int>(y)))
			for (MouseListener *listener : InputManager::INPUT_MANAGER.get_mouse_listeners())
			{
				listener->on_mouse_position(ionic_window.value(), x, y);
			}
	}

	void mouse_enter_callback(GLFWwindow *window, int entered)
	{
		auto ionic_window = window::get_by_handle(window);
		if (!ionic_window)
			ionic_window = {Window{window}};
		for (MouseListener *listener : InputManager::INPUT_MANAGER.get_mouse_listeners())
		{
			if (entered)
				listener->on_mouse_enter(ionic_window.value());
			else
				listener->on_mouse_exit(ionic_window.value());
		}
	}

	void scroll(GLFWwindow *window, double xoffset, double yoffset)
	{
		auto ionic_window = window::get_by_handle(window);
		if (!ionic_window)
			ionic_window = {Window{window}};
		for (MouseListener *listener : InputManager::INPUT_MANAGER.get_mouse_listeners())
		{
			listener->on_mouse_scroll(ionic_window.value(), xoffset, yoffset);
		}
	}

	void InputManager::init(bool use_controllers)
	{
		glfwSetJoystickCallback(invoke_controller_base_event);

		if (use_controllers)
			input_thread = new std::thread{update_controllers};
	}

	void InputManager::shutdown()
	{
		if (input_thread != nullptr)
			input_thread->join();
	}

	void InputManager::attach_window(const Window &window)
	{
		glfwSetKeyCallback(window.get_handle(), key_callback);
		glfwSetCharCallback(window.get_handle(), char_callback);
		glfwSetMouseButtonCallback(window.get_handle(), mouse_button_callback);
		glfwSetCursorPosCallback(window.get_handle(), mouse_position_callback);
		glfwSetCursorEnterCallback(window.get_handle(), mouse_enter_callback);
		glfwSetScrollCallback(window.get_handle(), scroll);
	}

	std::string InputManager::get_key_name(int key, int scancode)
	{
		auto key_name = glfwGetKeyName(key, scancode);
		if (key_name == nullptr)
			return "NULL";
		return {key_name};
	}

	void InputManager::add_keyboard_listener(KeyboardListener *listener)
	{
		if (!has_keyboard_listener(listener))
			keyboard_listeners.push_back(listener);
	}

	bool InputManager::remove_keyboard_listener(KeyboardListener *listener)
	{
		if (!has_keyboard_listener(listener))
			return false;
		keyboard_listeners.erase(std::remove(keyboard_listeners.begin(), keyboard_listeners.end(), listener),
								 keyboard_listeners.end());
		return true;
	}

	bool InputManager::has_keyboard_listener(KeyboardListener *listener)
	{
		return std::find(keyboard_listeners.begin(), keyboard_listeners.end(), listener) !=
			   keyboard_listeners.end();
	}

	std::vector<KeyboardListener *> InputManager::get_keyboard_listeners() const
	{
		return keyboard_listeners;
	}

	void InputManager::add_mouse_listener(MouseListener *listener)
	{
		if (!has_mouse_listener(listener))
			mouse_listeners.push_back(listener);
	}

	bool InputManager::remove_mouse_listener(MouseListener *listener)
	{
		if (!has_mouse_listener(listener))
			return false;
		mouse_listeners.erase(std::remove(mouse_listeners.begin(), mouse_listeners.end(), listener),
							  mouse_listeners.end());
		return true;
	}

	bool InputManager::has_mouse_listener(MouseListener *listener)
	{
		return std::find(mouse_listeners.begin(), mouse_listeners.end(), listener) !=
			   mouse_listeners.end();
	}

	std::vector<MouseListener *> InputManager::get_mouse_listeners() const
	{
		return mouse_listeners;
	}

	Controller *InputManager::get_controller(uint8_t id)
	{
		if (id > controllers.size() && id > GLFW_JOYSTICK_LAST)
			throw std::out_of_range("Cannot get a Controller with the ID: " + std::to_string(id));
		return controllers[id];
	}

	std::vector<Controller *> InputManager::get_controllers() const
	{
		return controllers;
	}

	void InputManager::add_controller_base_listener(ControllerBaseListener *listener)
	{
		if (!has_controller_base_listener(listener))
			controller_base_listeners.push_back(listener);
	}

	bool InputManager::remove_controller_base_listener(ControllerBaseListener *listener)
	{
		if (!has_controller_base_listener(listener))
			return false;
		controller_base_listeners
				.erase(std::remove(controller_base_listeners.begin(), controller_base_listeners.end(), listener),
					   controller_base_listeners.end());
		return true;
	}

	bool InputManager::has_controller_base_listener(ControllerBaseListener *listener)
	{
		return std::find(controller_base_listeners.begin(), controller_base_listeners.end(), listener) !=
			   controller_base_listeners.end();
	}

	std::vector<ControllerBaseListener *> InputManager::get_controller_base_listeners() const
	{
		return controller_base_listeners;
	}

	void InputManager::add_controller_input_listener(ControllerInputListener *listener)
	{
		if (!has_controller_input_listener(listener))
			controller_input_listeners.push_back(listener);
	}

	bool InputManager::remove_controller_input_listener(ControllerInputListener *listener)
	{
		if (!has_controller_input_listener(listener))
			return false;
		controller_input_listeners
				.erase(std::remove(controller_input_listeners.begin(), controller_input_listeners.end(), listener),
					   controller_input_listeners.end());
		return true;
	}

	bool InputManager::has_controller_input_listener(ControllerInputListener *listener)
	{
		return std::find(controller_input_listeners.begin(), controller_input_listeners.end(), listener) !=
			   controller_input_listeners.end();
	}

	std::vector<ControllerInputListener *> InputManager::get_controller_input_listeners() const
	{
		return controller_input_listeners;
	}

	InputManager InputManager::INPUT_MANAGER{};
}