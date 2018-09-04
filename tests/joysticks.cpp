/*
 * Copyright © 2018 AperLambda <aperlambda@gmail.com>
 *
 * This file is part of IonicEngine.
 *
 * Licensed under the MIT license. For more information,
 * see the LICENSE file.
 */

#include <lambdacommon/system/system.h>
#include <ionicengine/input/inputmanager.h>
#include <ionicengine/graphics/screen.h>
#include <fstream>
#include <sstream>

using namespace std;
using namespace lambdacommon;
using namespace ionicengine;

static void error_callback(int error, const char *description)
{
	fprintf(stderr, "Error (%i): %s\n", error, description);
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
private:
	ScreenManager *screenManager;
public:
	explicit ControllerInputListenerImpl(ScreenManager *screenManager) : screenManager(screenManager)
	{}

	void onButtonPress(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing A" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_GREEN);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing B" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_RED);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing X" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_BLUE);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Pressing Y" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(lambdacommon::getColorByIntRGBA(255, 255, 0));
		}
	}

	void onButtonRepeat(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating A" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_GREEN);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating B" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_RED);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating X" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_BLUE);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Repeating Y" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(lambdacommon::getColorByIntRGBA(255, 255, 0));
		}
	}

	void onButtonRelease(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing A" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_BLACK);
			//backgroundColor = Color::BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing B" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_BLACK);
			//backgroundColor = Color::COLOR_BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing X" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_BLACK);
			//backgroundColor = Color::BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.getId()) << "] Releasing Y" << endl;
			screenManager->getActiveScreen()->setBackgroundColor(Color::COLOR_BLACK);
			//backgroundColor = Color::BLACK;
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

/*class OverlayFPS : public Overlay
{
private:
	Font font;
public:
	OverlayFPS(Font font) : font(std::move(font))
	{}

	int fps = 0;

	void draw(Graphics *graphics) override
	{
		graphics->setColor(Color::COLOR_WHITE);
		graphics->drawText(font, 2, 3, "FPS: " + to_string(fps));
	}

	void update() override
	{

	}
};*/

int main()
{
	cout << "Starting joysticks.cpp with IonicEngine v" << ionicengine::getVersion() << "..." << endl;
	terminal::setup();
	IonicOptions options;
	options.debug = true;
	ionicengine::init(options);
	glfwSetErrorCallback(error_callback);

	auto gamecontrollerdb = fs::getCurrentWorkingDirectory() / "gamecontrollerdb.txt";
	if (!gamecontrollerdb.exists())
	{
		cerr << "gamecontrollerdb.txt not found." << endl;
		ionicengine::shutdown();
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

	glfwUpdateGamepadMappings(mappings.str().c_str());

	for (Controller *controller : InputManager::INPUT_MANAGER.getControllers())
	{
		controller->setAxisInverted(3, true);
		cout << "Controller{" << terminal::YELLOW << "\"id\"" << terminal::RESET << ':' << terminal::CYAN
			 << to_string(controller->getId()) << terminal::RESET << ',' << terminal::YELLOW << "\"name\""
			 << terminal::RESET << ':' << terminal::CYAN << '"' << controller->getName() << '"' << terminal::RESET
			 << ','
			 << terminal::YELLOW << "\"guid\'" << terminal::RESET << ':' << terminal::CYAN << '"'
			 << controller->getGUID() << '"' << terminal::RESET << ','
			 << terminal::YELLOW << "\"isConnected\"" << terminal::RESET << ':' << terminal::CYAN
			 << lambdastring::to_string(controller->isConnected()) << terminal::RESET << ','
			 << terminal::YELLOW << "\"isGamepad\"" << terminal::RESET << ':' << terminal::CYAN
			 << lambdastring::to_string(controller->isGamepad()) << terminal::RESET << '}' << endl;
	}

	ScreenManager screenManager{};

	ControllerBaseListenerImpl listener;
	InputManager::INPUT_MANAGER.addControllerBaseListener(&listener);
	ControllerInputListenerImpl inputListener{&screenManager};
	InputManager::INPUT_MANAGER.addControllerInputListener(&inputListener);

	auto windowOptions = ionicengine::DEFAULT_WINDOW_OPTIONS;
	windowOptions.resizable = false;
	windowOptions.context_version_major = 3;
	windowOptions.context_version_minor = 3;
	windowOptions.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
	windowOptions.samples = 4;
	auto window = window::createWindow("IonicEngine - Game Controllers", 512, 512, windowOptions);
	window.requestContext();
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (err != GLEW_OK)
	{
		ionicengine::shutdown();
		return err;
	}

	glEnable(GL_MULTISAMPLE);

	auto font = ionicengine::getFontManager().loadFont(string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	OverlayFPS overlay{font.value()};
	screenManager.registerOverlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screenManager.addActiveOverlay(IONICENGINE_OVERLAYS_FPS);

	getGraphicsManager()->init();

	screenManager.attachWindow(window);

	screenManager.startLoop();

	/*static double limitFPS = 1.0 / 144.0;

	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0, updates = 0;

	while (!window.shouldClose())
	{
		screenManager.render();
		//glClearColor(backgroundColor.red(), backgroundColor.green(), backgroundColor.blue(), backgroundColor.alpha());
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// - Measure time
		nowTime = glfwGetTime();
		deltaTime += (nowTime - lastTime) / limitFPS;
		lastTime = nowTime;

		// - Only update at 60 frames / s
		while (deltaTime >= 1.0)
		{
			screenManager.update();   // - Update function
			updates++;
			deltaTime--;
		}
		// - Render at maximum possible frames
		frames++;

		glfwSwapBuffers(window.getHandle());
		glfwPollEvents();

		// - Reset after one second
		if (glfwGetTime() - timer > 1.0)
		{
			timer++;
			window.setTitle("IonicEngine - Game Controllers - " + to_string(frames) + "FPS");
			//overlay.fps = frames;
			updates = 0, frames = 0;
		}
	}*/

	ionicengine::shutdown();
	glfwTerminate();

	return EXIT_SUCCESS;
}