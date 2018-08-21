/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
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
#include <lambdacommon/graphics/color.h>

using namespace std;
using namespace lambdacommon;
using namespace ionicengine;

lambdacommon::Color backgroundColor = Color::BLACK;

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error: %s\n", description);
}

class ControllerBaseListenerImpl : public ControllerBaseListener
{
public:
	void connect(Controller *controller) override
	{
		cout << "Controller connected: id -> " << terminal::LIGHT_YELLOW << to_string(controller->getId())
			 << terminal::RESET << ", name -> '" << controller->getName() << "', guid -> '" << terminal::LIGHT_YELLOW
			 << controller->getGUID() << terminal::RESET << "'." << endl;
	}

	void disconnect(Controller *controller) override
	{
		cout << "Controller disconnected: id -> " << terminal::LIGHT_YELLOW << to_string(controller->getId())
			 << terminal::RESET << endl;
	}
};

class ControllerInputListenerImpl : public ControllerInputListener
{
public:
	void onButtonPress(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing A" << endl;
			backgroundColor = Color::GREEN;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing B" << endl;
			backgroundColor = Color::RED;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing X" << endl;
			backgroundColor = Color::BLUE;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing Y" << endl;
			backgroundColor = lambdacommon::getColorByIntRGBA(255, 255, 0);
		}
	}

	void onButtonRepeat(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating A" << endl;
			backgroundColor = Color::GREEN;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating B" << endl;
			backgroundColor = Color::RED;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating X" << endl;
			backgroundColor = Color::BLUE;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating Y" << endl;
			backgroundColor = lambdacommon::getColorByIntRGBA(255, 255, 0);
		}
	}

	void onButtonRelease(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing A" << endl;
			backgroundColor = Color::BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing B" << endl;
			backgroundColor = Color::BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing X" << endl;
			backgroundColor = Color::BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing Y" << endl;
			backgroundColor = Color::BLACK;
		}
	}

	void onAxisMove(const Controller &controller, uint8_t axis, float value) override
	{
		cout << "[Controller " << to_string(controller.getId()) << "] Moving axis " << to_string(axis) << " with value "
			 << to_string(value) << " is inverted " << lambdastring::to_string(controller.isAxisInverted(axis)) << endl;
	}

	void onAxisRelease(const Controller &controller, uint8_t axis) override
	{
		cout << "[Controller " << to_string(controller.getId()) << "] Releasing axis " << to_string(axis) << endl;
	}
};

int main()
{
	cout << "Starting joysticks.cpp with IonicEngine v" << ionicengine::getVersion() << "..." << endl;
	terminal::setup();
	ionicengine::init();
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

	for (Controller *controller : InputManager::INPUT_MANAGER.getControllers())
	{
		controller->setAxisInverted(3, true);
		cout << "Controller{" << terminal::YELLOW << "\"id\"" << terminal::RESET << ':' << terminal::CYAN
			 << to_string(controller->getId()) << terminal::RESET << ',' << terminal::YELLOW << "\"name\""
			 << terminal::RESET << ':' << terminal::CYAN << '"' << controller->getName() << '"' << terminal::RESET << ','
			 << terminal::YELLOW << "\"guid\'" << terminal::RESET << ':' << terminal::CYAN << '"'
			 << controller->getGUID() << '"' << terminal::RESET << ','
			 << terminal::YELLOW << "\"isConnected\"" << terminal::RESET << ':' << terminal::CYAN
			 << lambdastring::to_string(controller->isConnected()) << terminal::RESET << ','
			 << terminal::YELLOW << "\"isGamepad\"" << terminal::RESET << ':' << terminal::CYAN
			 << lambdastring::to_string(controller->isGamepad()) << terminal::RESET << '}' << endl;
	}

	ControllerBaseListenerImpl listener;
	InputManager::INPUT_MANAGER.addControllerBaseListener(&listener);
	ControllerInputListenerImpl inputListener;
	InputManager::INPUT_MANAGER.addControllerInputListener(&inputListener);

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	auto window = glfwCreateWindow(512, 512, "IonicEngine - Game Controllers", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	static double limitFPS = 1.0 / 144.0;

	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0, updates = 0;

	while (!glfwWindowShouldClose(window))
	{
		glClearColor(backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(), backgroundColor.alpha());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// - Measure time
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / limitFPS;
		lastTime = nowTime;

		// - Only update at 60 frames / s
		while (deltaTime >= 1.0)
		{
			//update();   // - Update function
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

	ionicengine::shutdown();
	glfwDestroyWindow(window);
	glfwTerminate();

	return EXIT_SUCCESS;
}