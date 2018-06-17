/*
 * Copyright © 2018 AperLambda <aper.entertainment@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <lambdacommon/system/filesystem/filesystem.h>
#include <lambdacommon/system/system.h>
#include <ionicengine/input/inputmanager.h>
#include <fstream>
#include <sstream>
#include <GL/glew.h>

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

void update()
{
	GLFWgamepadstate state;
	if (glfwGetGamepadState(0, &state))
	{
		if (state.buttons[GLFW_GAMEPAD_BUTTON_A])
			cout << "button a pressed!" << endl;
	} else cout << "Not a gamepad!" << endl;
}

int main()
{
	cout << "Starting joysticks.cpp with IonicEngine v" << ionicengine::getVersion() << "..." << endl;
	terminal::setup();
	glfwSetErrorCallback(error_callback);

	auto gamecontrollerdb = fs::getCurrentWorkingDirectory() / "gamecontrollerdb.txt";
	if (!gamecontrollerdb.exists())
	{
		cerr << "gamecontrollerdb.txt not found." << endl;
		return EXIT_FAILURE;
	}
	ifstream read{};
	ostringstream mappings{};
	read.open(gamecontrollerdb.toAbsolute().toString());

	if (read.is_open())
	{
		for (string line; getline(read, line);)
		{
			mappings << line << endl;
		}
		read.close();
	}

	if (!glfwInit())
		return EXIT_FAILURE;

	glfwUpdateGamepadMappings(mappings.str().c_str());

	InputManager::INPUT_MANAGER.init();

	for (auto controller : InputManager::INPUT_MANAGER.getControllers())
	{
		cout << "Controller{" << terminal::YELLOW << "\"id\"" << terminal::RESET << ':' << terminal::CYAN
		     << to_string(controller.getId()) << terminal::RESET << ',' << terminal::YELLOW << "\"name\""
		     << terminal::RESET << ':' << terminal::CYAN << '"' << controller.getName() << '"' << terminal::RESET << ','
		     << terminal::YELLOW << "\"guid\'" << terminal::RESET << ':' << terminal::CYAN << '"'
		     << controller.getGUID() << '"' << terminal::RESET << ','
		     << terminal::YELLOW << "\"isConnected\"" << terminal::RESET << ':' << terminal::CYAN
		     << lambdastring::to_string(controller.isConnected()) << terminal::RESET << ','
		     << terminal::YELLOW << "\"isGamepad\"" << terminal::RESET << ':' << terminal::CYAN
		     << lambdastring::to_string(controller.isGamepad()) << terminal::RESET << '}' << endl;
	}

	ControllerBaseListenerImpl listener;

	InputManager::INPUT_MANAGER.addControllerBaseListener(&listener);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto window = glfwCreateWindow(512, 512, "IonicEngine - Game Controllers", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	static double limitFPS = 1.0 / 144.0;

	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0, updates = 0;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(0, 0, 0, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// - Measure time
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / limitFPS;
		lastTime = nowTime;

		// - Only update at 60 frames / s
		while (deltaTime >= 1.0)
		{
			update();   // - Update function
			updates++;
			deltaTime--;
		}
		// - Render at maximum possible frames
		frames++;

		glfwSwapBuffers(window);
		glfwPollEvents();

		// - Reset after one second
		if (glfwGetTime() - timer > 1.0)
		{
			timer++;
			glfwSetWindowTitle(window,
			                   (string("IonicEngine - Game Controllers - ") + to_string(frames) + "FPS").c_str());
			updates = 0, frames = 0;
		}
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}