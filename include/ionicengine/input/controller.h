/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#ifndef IONICENGINE_CONTROLLER_H
#define IONICENGINE_CONTROLLER_H

#include "../ionicengine.h"

namespace ionicengine
{
	class IONICENGINE_API Controller
	{
	private:
		const uint8_t joystick;
		bool axisInverted[6] = {false, false, false, false, false, false};

	public:
		explicit Controller(uint8_t id);

		/*! @brief Gets the ID of the controller.
		 *
		 * This function returns the ID of the controller.
		 *
		 * @return ID of the controller.
		 */
		uint8_t getId() const;

		/*! @brief Gets the name of the controller.
		 *
		 * This functions returns the native name of the controller.
		 *
		 * @return Controller's name.
		 */
		std::string getName() const;

		/*! @brief Gets the GUID of the controller.
		 *
		 * The GUID is what connects a joystick to a gamepad mapping.  A connected
		 *  joystick will always have a GUID even if there is no gamepad mapping
		 *  assigned to it.
		 *
		 * @return Controller's GUID.
		 */
		std::string getGUID() const;

		/*! @brief Checks whether the controller is connected or not.
		 *
		 * This function checks whether the constroller is connected with GLFW.
		 *
		 * @return True if the controller is connected else false.
		 */
		bool isConnected() const;

		/*! @brief Returns whether the controller has a gamepad mapping.
		 *
		 * @return True if the controller has a gamepad mapping else false.
		 */
		bool isGamepad() const;

		/*! @brief Sets whether the axis of the controller is inverted or not.
		 *
		 * @param axis The axis ID, between 0 and 5.
		 * @param inverted True if the axis is inverted else false.
		 */
		void setAxisInverted(uint8_t axis, bool inverted);

		/*! @brief Returns whether the axis of the controller is inverted or not.
		 *
		 * @param axis The axis ID, between 0 and 5.
		 * @return True if the axis is inverted else false.
		 */
		bool isAxisInverted(uint8_t axis) const;
	};
}

#endif //IONICENGINE_CONTROLLER_H