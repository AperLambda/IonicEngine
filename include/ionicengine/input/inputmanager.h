/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_INPUTMANAGER_H
#define IONICENGINE_INPUTMANAGER_H

#include "controller.h"
#include "../window/window.h"
#include <vector>
#include <thread>

namespace ionicengine
{
	enum IONICENGINE_API InputAction
	{
		PRESS = GLFW_PRESS,
		REPEAT = GLFW_REPEAT,
		RELEASE = GLFW_RELEASE
	};

	class IONICENGINE_API ControllerBaseListener
	{
	public:
		virtual void connect(Controller *controller) = 0;

		virtual void disconnect(Controller *controller) = 0;
	};

	class IONICENGINE_API ControllerInputListener
	{
	public:
		virtual void on_button_press(const Controller &controller, uint8_t button) = 0;

		virtual void on_button_repeat(const Controller &controller, uint8_t button) = 0;

		virtual void on_button_release(const Controller &controller, uint8_t button) = 0;

		virtual void on_axis_move(const Controller &controller, uint8_t axis, float value) = 0;

		virtual void on_axis_release(const Controller &controller, uint8_t axis) = 0;
	};

	class IONICENGINE_API KeyboardListener
	{
	public:
		virtual void on_key_input(Window &window, int key, int scancode, InputAction action, int mods) = 0;

		virtual void on_char_input(Window &window, char32_t codepoint) = 0;
	};

	class IONICENGINE_API MouseListener
	{
	public:
		virtual void on_mouse_button(Window &window, int button, InputAction action, int mods) = 0;

		virtual void on_mouse_position(Window &window, double x, double y) = 0;
		
		virtual void on_mouse_enter(Window &window) = 0;
		
		virtual void on_mouse_exit(Window &window) = 0;

		virtual void on_mouse_scroll(Window &window, double xoffset, double yoffset) = 0;
	};

	class IONICENGINE_API InputManager
	{
	private:
		std::vector<Controller *> controllers;

		std::vector<KeyboardListener *> keyboard_listeners;
		std::vector<MouseListener *> mouse_listeners;
		std::vector<ControllerBaseListener *> controller_base_listeners;
		std::vector<ControllerInputListener *> controller_input_listeners;

		std::thread *input_thread = nullptr;

		InputManager();

	public:
		~InputManager();

		/*! @brief Initializes the input manager.
		 *
		 * This function inits the input manager.
		 */
		void init(bool use_controllers = true);

		/*! @biref Shutdown the input manager.
		 * DO NOT CALL THIS ON YOURSELF!
		 */
		void shutdown();

		/*!
		 * Attaches a window to the input manager.
		 * @param window Window to attach.
		 */
		void attach_window(const Window &window);

		/*!
		 * Gets the name of a keyboard key.
		 * @param key The key to query.
		 * @param scancode The scancode of the key to query.
		 * @return The UTF-8 encoded, layout-specific name of the key, or {@code NULL}
		 */
		std::string get_key_name(int key, int scancode);

		/*!
		 * Adds a keyboard listener.
		 * @param listener The pointer of the listener to add.
		 */
		void add_keyboard_listener(KeyboardListener *listener);

		/*!
		 * Removes a keyboard listener.
		 * @param listener The pointer of the listener to add.
		 */
		bool remove_keyboard_listener(KeyboardListener *listener);

		/*!
		 * Checks whether a keyboard listener is registered.
		 * @param listener The pointer of the listener to check.
		 * @return True if the listener was found else false.
		 */
		bool has_keyboard_listener(KeyboardListener *listener);

		std::vector<KeyboardListener *> get_keyboard_listeners() const;

		/*!
		 * Adds a mouse listener.
		 * @param listener The pointer of the listener to add.
		 */
		void add_mouse_listener(MouseListener *listener);

		/*!
		 * Removes a mouse listener.
		 * @param listener The pointer of the listener to add.
		 */
		bool remove_mouse_listener(MouseListener *listener);

		/*!
		 * Checks whether a mouse listener is registered.
		 * @param listener The pointer of the listener to check.
		 * @return True if the listener was found else false.
		 */
		bool has_mouse_listener(MouseListener *listener);

		std::vector<MouseListener *> get_mouse_listeners() const;

		/*! @brief Gets an controller by his defined ID.
		 *
		 * This function returns an controller by his defined ID.
		 * The ID is between 0 and GLFW_JOYSTICK_LAST.
		 *
		 * @param id ID of the controller.
		 *
		 * @return The instance of the controller.
		 */
		Controller *get_controller(uint8_t id);

		/*! @brief Gets all controllers.
		 *
		 * This function returns an vector of all controllers.
		 *
		 * @return All controllers.
		 */
		std::vector<Controller *> get_controllers() const;

		/*! @brief Adds an basic listener.
		 *
		 * This function adds a new listener.
		 *
		 * @param listener The pointer of the listener to add.
		 */
		void add_controller_base_listener(ControllerBaseListener *listener);

		/*! @brief Removes an basic listener.
		 *
		 * This function removes a new listener.
		 *
		 * @param listener The pointer of the listener to remove.
		 *
		 * @return True whether the listener was removed else false.
		 */
		bool remove_controller_base_listener(ControllerBaseListener *listener);

		/*! @brief Checks whether an basic listener is registered.
		 *
		 * This function checks whether a basic listener is registered.
		 *
		 * @param listener The pointer of the listener to check.
		 *
		 * @return True whether the listener was found else false.
		 */
		bool has_controller_base_listener(ControllerBaseListener *listener);

		std::vector<ControllerBaseListener *> get_controller_base_listeners() const;

		/*! @brief Adds an input listener.
		 *
		 * This function adds a new listener.
		 *
		 * @param listener The pointer of the listener to add.
		 */
		void add_controller_input_listener(ControllerInputListener *listener);

		/*! @brief Removes an input listener.
		 *
		 * @param listener The pointer of the listener to remove.
		 *
		 * @return True whether the listener was removed else false.
		 */
		bool remove_controller_input_listener(ControllerInputListener *listener);

		/*! @brief Checks whether an input listener is registered.
		 *
		 * This function checks whether an input listener is registered.
		 *
		 * @param listener The pointer of the listener to check.
		 *
		 * @return True whether the listener was found else false.
		 */
		bool has_controller_input_listener(ControllerInputListener *listener);

		std::vector<ControllerInputListener *> get_controller_input_listeners() const;

		// For singleton.
		InputManager(InputManager const &) = delete;

		// For singleton.
		void operator=(InputManager const &)  = delete;

		static InputManager INPUT_MANAGER;
	};
}

#endif //IONICENGINE_INPUTMANAGER_H