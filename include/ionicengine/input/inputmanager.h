/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
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

namespace ionicengine
{
	class IONICENGINE_API ControllerBaseListener
	{
	public:
		virtual void connect(Controller &controller) = 0;

		virtual void disconnect(Controller &controller) = 0;
	};

	class IONICENGINE_API InputManager
	{
	private:
		std::vector<Controller> controllers;

		std::vector<ControllerBaseListener *> controllerBaseListeners;

		InputManager();

	public:
		/*! @brief Initializes the input manager.
		 *
		 * This function inits the input manager.
		 */
		void init();

		/*! @brief Gets an controller by his defined ID.
		 *
		 * This function returns an controller by his defined ID.
		 * The ID is between 0 and GLFW_JOYSTICK_LAST.
		 *
		 * @param id ID of the controller.
		 *
		 * @return The instance of the controller.
		 */
		Controller getController(uint8_t id);

		/*! @brief Gets all controllers.
		 *
		 * This function returns an vector of all controllers.
		 *
		 * @return All controllers.
		 */
		std::vector<Controller> getControllers() const;

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
		 * This function checks wether an basic listener is registered.
		 *
		 * @param listener The pointer of the listener to check.
		 *
		 * @return True whether the listener was found else false.
		 */
		bool hasControllerBaseListener(ControllerBaseListener *listener);

		std::vector<ControllerBaseListener *> getControllerBaseListeners() const;

		// For singleton.
		InputManager(InputManager const &) = delete;

		// For singleton.
		void operator=(InputManager const &)  = delete;

		static InputManager INPUT_MANAGER;
	};
}

#endif //IONICENGINE_INPUTMANAGER_H