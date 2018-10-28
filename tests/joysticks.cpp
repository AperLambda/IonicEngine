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
		cout << "Controller connected: id -> " << terminal::LIGHT_YELLOW << to_string(controller->get_id())
			 << terminal::RESET << ", name -> '" << controller->get_name() << "', guid -> '" << terminal::LIGHT_YELLOW
			 << controller->get_guid() << terminal::RESET << "'." << endl;
	}

	void disconnect(Controller *controller) override
	{
		cout << "Controller disconnected: id -> " << terminal::LIGHT_YELLOW << to_string(controller->get_id())
			 << terminal::RESET << endl;
	}
};

class ControllerInputListenerImpl : public ControllerInputListener
{
private:
	ScreenManager *screen_manager;
public:
	explicit ControllerInputListenerImpl(ScreenManager *screen_manager) : screen_manager(screen_manager)
	{}

	void on_button_press(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Pressing A" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_GREEN);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Pressing B" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_RED);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Pressing X" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_BLUE);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Pressing Y" << endl;
			screen_manager->get_active_screen()->set_background_color(lambdacommon::color::from_int_rgba(255, 255, 0));
		}
	}

	void on_button_repeat(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Repeating A" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_GREEN);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Repeating B" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_RED);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Repeating X" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_BLUE);
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Repeating Y" << endl;
			screen_manager->get_active_screen()->set_background_color(lambdacommon::color::from_int_rgba(255, 255, 0));
		}
	}

	void on_button_release(const Controller &controller, uint8_t button) override
	{
		if (button == GLFW_GAMEPAD_BUTTON_A)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Releasing A" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_BLACK);
			//backgroundColor = Color::BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_B)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Releasing B" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_BLACK);
			//backgroundColor = Color::COLOR_BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_X)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Releasing X" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_BLACK);
			//backgroundColor = Color::BLACK;
		}
		else if (button == GLFW_GAMEPAD_BUTTON_Y)
		{
			cout << "[Controller " << to_string(controller.get_id()) << "] Releasing Y" << endl;
			screen_manager->get_active_screen()->set_background_color(Color::COLOR_BLACK);
			//backgroundColor = Color::BLACK;
		}
	}

	void on_axis_move(const Controller &controller, uint8_t axis, float value) override
	{
		cout << "[Controller " << to_string(controller.get_id()) << "] Moving axis " << to_string(axis) << " with value "
			 << to_string(value) << " is inverted " << lstring::to_string(controller.is_axis_inverted(axis)) << endl;
	}

	void on_axis_release(const Controller &controller, uint8_t axis) override
	{
		cout << "[Controller " << to_string(controller.get_id()) << "] Releasing axis " << to_string(axis) << endl;
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
		graphics->set_color(Color::COLOR_WHITE);
		graphics->draw_text(font, 2, 3, "FPS: " + to_string(fps));
	}

	void update() override
	{

	}
};*/

int main()
{
	cout << "Starting joysticks.cpp with IonicEngine v" << ionicengine::get_version() << "..." << endl;
	terminal::setup();
	IonicOptions options;
	options.debug = true;
	ionicengine::init(options);
	glfwSetErrorCallback(error_callback);

	auto gamecontrollerdb = fs::get_current_working_directory() / "gamecontrollerdb.txt";
	if (!gamecontrollerdb.exists())
	{
		cerr << "gamecontrollerdb.txt not found." << endl;
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}
	ifstream read{};
	ostringstream mappings{};
	read.open(gamecontrollerdb.to_absolute().to_string());

	if (read.is_open())
	{
		for (string line; getline(read, line);)
		{
			mappings << line << endl;
		}
		read.close();
	}

	glfwUpdateGamepadMappings(mappings.str().c_str());

	for (Controller *controller : InputManager::INPUT_MANAGER.get_controllers())
	{
		controller->set_axis_inverted(3, true);
		cout << "Controller{" << terminal::YELLOW << "\"id\"" << terminal::RESET << ':' << terminal::CYAN
			 << to_string(controller->get_id()) << terminal::RESET << ',' << terminal::YELLOW << "\"name\""
			 << terminal::RESET << ':' << terminal::CYAN << '"' << controller->get_name() << '"' << terminal::RESET
			 << ','
			 << terminal::YELLOW << "\"guid\'" << terminal::RESET << ':' << terminal::CYAN << '"'
			 << controller->get_guid() << '"' << terminal::RESET << ','
			 << terminal::YELLOW << "\"is_connected\"" << terminal::RESET << ':' << terminal::CYAN
			 << lstring::to_string(controller->is_connected()) << terminal::RESET << ','
			 << terminal::YELLOW << "\"is_gamepad\"" << terminal::RESET << ':' << terminal::CYAN
			 << lstring::to_string(controller->is_gamepad()) << terminal::RESET << '}' << endl;
	}

	ScreenManager screen_manager{};

	ControllerBaseListenerImpl listener;
	InputManager::INPUT_MANAGER.add_controller_base_listener(&listener);
	ControllerInputListenerImpl input_listener{&screen_manager};
	InputManager::INPUT_MANAGER.add_controller_input_listener(&input_listener);

	auto window_options = ionicengine::DEFAULT_WINDOW_OPTIONS;
	window_options.resizable = false;
	window_options.context_version_major = 3;
	window_options.context_version_minor = 3;
	window_options.opengl_profile = GLFW_OPENGL_CORE_PROFILE;
	window_options.samples = 4;
	auto window = window::create_window("IonicEngine - Game Controllers", 512, 512, window_options);
	window.request_context();
	if (!ionicengine::post_init())
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	glEnable(GL_MULTISAMPLE);

	auto font = ionicengine::get_font_manager()->load_font(string{"Roboto.ttf"}, 14);
	if (!font)
	{
		ionicengine::shutdown();
		return EXIT_FAILURE;
	}

	OverlayFPS overlay{font.value()};
	screen_manager.register_overlay(IONICENGINE_OVERLAYS_FPS, &overlay);
	screen_manager.add_active_overlay(IONICENGINE_OVERLAYS_FPS);

	get_graphics_manager()->init();

	screen_manager.attach_window(window);

	screen_manager.start_loop();

	/*static double limitFPS = 1.0 / 144.0;

	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;
	int frames = 0, updates = 0;

	while (!window.should_close())
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

		glfwSwapBuffers(window.get_handle());
		glfwPollEvents();

		// - Reset after one second
		if (glfwGetTime() - timer > 1.0)
		{
			timer++;
			window.set_title("IonicEngine - Game Controllers - " + to_string(frames) + "FPS");
			//overlay.fps = frames;
			updates = 0, frames = 0;
		}
	}*/

	ionicengine::shutdown();
	glfwTerminate();

	return EXIT_SUCCESS;
}