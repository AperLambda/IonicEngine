/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <lambdacommon/system/system.h>
#include <ionicengine/input/inputmanager.h>
#include <ionicengine/graphics/graphics.h>

using namespace std;
using namespace lambdacommon;
using namespace ionicengine;

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

class ControllerBaseListenerImpl : public ControllerBaseListener
{
public:
	void connect(Controller &controller) override
	{
		cout << "Controller connected: id -> " << terminal::LIGHT_YELLOW << to_string(controller.getId())
		     << terminal::RESET << ", name -> '" << controller.getName() << "', guid -> '" << terminal::LIGHT_YELLOW
		     << controller.getGUID() << terminal::RESET << "'." << endl;
	}

	void disconnect(Controller &controller) override
	{
		cout << "Controller disconnected: id -> " << terminal::LIGHT_YELLOW << to_string(controller.getId())
		     << terminal::RESET << endl;
	}
};

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

int main()
{
	cout << "Starting joysticks.cpp with IonicEngine v" << ionicengine::getVersion() << "..." << endl;
	terminal::setup();
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		return EXIT_FAILURE;

	InputManager::INPUT_MANAGER.init();

	for (auto controller : InputManager::INPUT_MANAGER.getControllers())
	{
		cout << "Controller{" << terminal::YELLOW << "\"id\"" << terminal::RESET << ':' << terminal::CYAN
		     << to_string(controller.getId()) << terminal::RESET << ',' << terminal::YELLOW << "\"name\""
		     << terminal::RESET << ':' << terminal::CYAN << '"' << controller.getName() << '"' << terminal::RESET << ','
		     << terminal::YELLOW << "\"guid\'" << terminal::RESET << ':' << terminal::CYAN << '"'
		     << controller.getGUID() << '"' << terminal::RESET << ','
		     << terminal::YELLOW << "\"isConnected\"" << terminal::RESET << ':' << terminal::CYAN
		     << lambdastring::to_string(controller.isConnected()) << terminal::RESET << '}' << endl;
	}

	ControllerBaseListenerImpl listener;

	InputManager::INPUT_MANAGER.addControllerBaseListener(&listener);

	bool running = true;

	GraphicsManager gm{};
	gm.registerGraphics({"domain:uwu"}, []() { return new Graphics(); });

	while (running)
	{
		glfwPollEvents();
	}

	return EXIT_SUCCESS;
}