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
#include <vector>
#include <thread>

namespace ionicengine
{
	class IONICENGINE_API ControllerBaseListener
	{
	public:
		virtual void connect(Controller *controller) = 0;

		virtual void disconnect(Controller *controller) = 0;
	};

	class IONICENGINE_API ControllerInputListener
	{
	public:
		virtual void onButtonPress(const Controller &controller, uint8_t button) = 0;

		virtual void onButtonRepeat(const Controller &controller, uint8_t button) = 0;

		virtual void onButtonRelease(const Controller &controller, uint8_t button) = 0;

		virtual void onAxisMove(const Controller &controller, uint8_t axis, float value) = 0;

		virtual void onAxisRelease(const Controller &controller, uint8_t axis) = 0;
	};

	class IONICENGINE_API InputManager
	{
	private:
		std::vector<Controller *> controllers;

		std::vector<ControllerBaseListener *> controllerBaseListeners;
		std::vector<ControllerInputListener *> controllerInputListeners;

		std::thread *inputThread = nullptr;

		InputManager();

	public:
		~InputManager();

		/*! @brief Initializes the input manager.
		 *
		 * This function inits the input manager.
		 */
		void init();

		/*! @biref Shutdown the input manager.
		 * DO NOT CALL THIS ON YOURSELF!
		 */
		void shutdown();

		/*! @brief Gets an controller by his defined ID.
		 *
		 * This function returns an controller by his defined ID.
		 * The ID is between 0 and GLFW_JOYSTICK_LAST.
		 *
		 * @param id ID of the controller.
		 *
		 * @return The instance of the controller.
		 */
		Controller *getController(uint8_t id);

		/*! @brief Gets all controllers.
		 *
		 * This function returns an vector of all controllers.
		 *
		 * @return All controllers.
		 */
		std::vector<Controller *> getControllers() const;

		/*! @brief Adds an basic listener.
		 *
		 * This function adds a new listener.
		 *
		 * @param listener The pointer of the listener to add.
		 */
		void addControllerBaseListener(ControllerBaseListener *listener);

		/*! @brief Removes an basic listener.
		 *
		 * This function removes a new listener.
		 *
		 * @param listener The pointer of the listener to remove.
		 *
		 * @return True whether the listener was removed else false.
		 */
		bool removeControllerBaseListener(ControllerBaseListener *listener);

		/*! @brief Checks whether an basic listener is registered.
		 *
		 * This function checks whether a basic listener is registered.
		 *
		 * @param listener The pointer of the listener to check.
		 *
		 * @return True whether the listener was found else false.
		 */
		bool hasControllerBaseListener(ControllerBaseListener *listener);

		std::vector<ControllerBaseListener *> getControllerBaseListeners() const;

		/*! @brief Adds an input listener.
		 *
		 * This function adds a new listener.
		 *
		 * @param listener The pointer of the listener to add.
		 */
		void addControllerInputListener(ControllerInputListener *listener);

		/*! @brief Removes an input listener.
		 *
		 * @param listener The pointer of the listener to remove.
		 *
		 * @return True whether the listener was removed else false.
		 */
		bool removeControllerInputListener(ControllerInputListener *listener);

		/*! @brief Checks whether an input listener is registered.
		 *
		 * This function checks whether an input listener is registered.
		 *
		 * @param listener The pointer of the listener to check.
		 *
		 * @return True whether the listener was found else false.
		 */
		bool hasControllerInputListener(ControllerInputListener *listener);

		std::vector<ControllerInputListener *> getControllerInputListeners() const;

		// For singleton.
		InputManager(InputManager const &) = delete;

		// For singleton.
		void operator=(InputManager const &)  = delete;

		static InputManager INPUT_MANAGER;
	};
}

#endif //IONICENGINE_INPUTMANAGER_H